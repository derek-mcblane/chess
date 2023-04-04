#include "bit_board.h"
#include "grid_view.h"
#include "pieces.h"
#include "timing.h"

#include "sdlpp.h"
#include "sdlpp_image.h"

#include <gsl/gsl>
#include <spdlog/cfg/env.h>
#include <spdlog/spdlog.h>

#include <chrono>
#include <exception>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <thread>

using namespace chess;
using Position = dm::Vec2<int>;
using Point = sdl::Point<int>;

template <>
struct fmt::formatter<SDL_MouseButtonEvent> : fmt::formatter<std::string>
{
    auto format(SDL_MouseButtonEvent event, format_context& ctx) -> decltype(ctx.out())
    {
        return format_to(
            ctx.out(),
            "[SDL_MouseButtonEvent"
            "type={}, timestamp={}, windowID={}, which={}, button={}, state={}, clicks={}, padding1={}, x={}, y={}]",
            event.type,
            event.timestamp,
            event.windowID,
            event.which,
            event.button,
            event.state,
            event.clicks,
            event.padding1,
            event.x,
            event.y
        );
    }
};

template <>
struct fmt::formatter<Point> : fmt::formatter<std::string>
{
    auto format(Point point, format_context& ctx) -> decltype(ctx.out())
    {
        return format_to(ctx.out(), "[Point x={}, y={}]", point.x, point.y);
    }
};

template <>
struct fmt::formatter<Position> : fmt::formatter<std::string>
{
    auto format(Position point, format_context& ctx) -> decltype(ctx.out())
    {
        return format_to(ctx.out(), "[Position x={}, y={}]", point.x(), point.y());
    }
};

template <>
struct fmt::formatter<chess::Piece> : fmt::formatter<std::string>
{
    auto format(chess::Piece piece, format_context& ctx) -> decltype(ctx.out())
    {
        return format_to(
            ctx.out(), "{} {}", chess::piece_color_names.at(piece.color), chess::piece_type_names.at(piece.type)
        );
    }
};

Point transform_chess_to_grid_view(Position coordinate)
{
    return {.x = coordinate.y(), .y = coordinate.x()};
}

Position transform_grid_view_to_chess(Point coordinate)
{
    return {coordinate.y, coordinate.x};
}

template <typename Rep, typename Period>
Rep to_milliseconds(const std::chrono::duration<Rep, Period> duration)
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
}

template <typename Rectangle>
sdl::Point<sdl::rectangle_dimension_type<Rectangle>> rectangle_center(Rectangle rectangle)
{
    return {rectangle.x + rectangle.w / 2, rectangle.y + rectangle.h / 2};
}

namespace pallete {

[[maybe_unused]] static constexpr sdl::Color black{0x00, 0x00, 0x00, 0xFF};
[[maybe_unused]] static constexpr sdl::Color blue{0x00, 0x00, 0xFF, 0xFF};
[[maybe_unused]] static constexpr sdl::Color jasons_dumbass_blue{129, 152, 201, 0xFF};
[[maybe_unused]] static constexpr sdl::Color light_green{169, 216, 145, 0xFF};
[[maybe_unused]] static constexpr sdl::Color gray{0x0F, 0x0F, 0x0F, 0xFF};
[[maybe_unused]] static constexpr sdl::Color white{0xFF, 0xFF, 0xFF, 0xFF};

[[nodiscard]] sdl::Color color_with_alpha(sdl::Color color, Uint8 alpha)
{
    color.a = alpha;
    return color;
}

} // namespace pallete

class SpriteMapGrid
{
  public:
    SpriteMapGrid(Point map_size, Point n_elements) : pitch_{map_size.x / n_elements.x, map_size.y / n_elements.y} {}

    [[nodiscard]] sdl::Rectangle<int> get_region(Point coordinate) const;

  private:
    Point pitch_;
};

sdl::Rectangle<int> SpriteMapGrid::get_region(Point coordinate) const
{
    return {.x = coordinate.x * pitch_.x, .y = coordinate.y * pitch_.y, .w = pitch_.x, .h = pitch_.y};
}

template <typename T>
class SpriteGrid
{
  public:
    using CoordinateMap = std::map<T, Point>;

    SpriteGrid(Point texture_size, Point grid_size, CoordinateMap&& sprite_coordinates)
        : pitch_{texture_size.x / grid_size.x, texture_size.y / grid_size.y},
          coordinates_{std::move(sprite_coordinates)}
    {}

    [[nodiscard]] sdl::Rectangle<int> get_region(const T& sprite);

  private:
    Point pitch_;
    CoordinateMap coordinates_;

    [[nodiscard]] sdl::Rectangle<int> get_region(Point coordinate) const;
};

template <typename T>
sdl::Rectangle<int> SpriteGrid<T>::get_region(Point coordinate) const
{
    return {.x = coordinate.x * pitch_.x, .y = coordinate.y * pitch_.y, .w = pitch_.x, .h = pitch_.y};
}

template <typename T>
sdl::Rectangle<int> SpriteGrid<T>::get_region(const T& sprite)
{
    return get_region(coordinates_.at(sprite));
}

template <typename Event>
class EventHandlers
{
  public:
    void add_handler(std::function<void(const Event&)>&& handler)
    {
        handlers_.push_back(std::move(handler));
    }

    void call_all(const Event& event)
    {
        for (const auto handler : handlers_) {
            handler(event);
        }
    }

  private:
    std::vector<std::function<void(const Event&)>> handlers_;
};

class ChessApplication
{
  public:
    ChessApplication()
    {
        initialize_event_handlers();
        board_display_.set_on_cell_clicked_callback([this](const Point& point) { on_grid_cell_clicked(point); });
        process_events();
    }

    void on_grid_cell_clicked(const Point& point)
    {
        spdlog::debug("clicked cell {}", point);
        const auto coord = transform_grid_view_to_chess(point);

        const auto lock = std::lock_guard{pieces_mutex_};
        if (selected_piece_coordinate_.has_value() && selected_piece_valid_moves_.has_value()) {
            if (!selected_piece_valid_moves_->test(coord)) {
                spdlog::debug("invalid move");
            } else {
                spdlog::debug("moving from {} to {}", *selected_piece_coordinate_, coord);
                pieces_.move_piece(*selected_piece_coordinate_, coord);
            }
            selected_piece_coordinate_ = std::nullopt;
            selected_piece_valid_moves_ = std::nullopt;
        } else {
            if (pieces_.is_active_piece(coord)) {
                selected_piece_coordinate_ = std::optional{coord};
                selected_piece_valid_moves_ = pieces_.valid_moves(coord);
            }
        }
    }

    void initialize_event_handlers()
    {
        quit_event_handlers_.add_handler([this](const SDL_QuitEvent& event) { handle_quit_event(event); });
        mouse_button_down_event_handlers_.add_handler([this](const SDL_MouseButtonEvent& event) {
            board_display_.on_button_down(event);
        });
        mouse_button_up_event_handlers_.add_handler([this](const SDL_MouseButtonEvent& event) {
            board_display_.on_button_up(event);
        });
    }

    void run()
    {
        while (running_) {
            on_begin_frame();
            update();
            if (window_.shown()) {
                render();
            }
            process_events();
            on_end_frame();
        }
    }

  private:
    void on_begin_frame() {}

    void on_end_frame()
    {
        minimum_frame_delay_.end_interval();
        constexpr bool log_frame_duration = false;
        if (log_frame_duration) {
            const auto frame_duration = minimum_frame_delay_.previous_interval_duration();
            spdlog::debug("{} ms", to_milliseconds(frame_duration));
        }
    }

    void process_events()
    {
        while (auto event = sdl::poll_event()) {
            switch (event->type) {
            case SDL_QUIT:
                quit_event_handlers_.call_all(event->quit);
                break;
            case SDL_MOUSEBUTTONDOWN:
                mouse_button_down_event_handlers_.call_all(event->button);
                break;
            case SDL_MOUSEBUTTONUP:
                mouse_button_up_event_handlers_.call_all(event->button);
                break;
            default:
                break;
            }
        }
    }

    void handle_quit_event(const SDL_QuitEvent& event)
    {
        running_ = false;
    }

    void update()
    {
        const auto min_dimension_size = std::min(std::get<0>(window_.size()), std::get<1>(window_.size()));
        board_display_.pixel_size = {min_dimension_size, min_dimension_size};
    }

    void render_board()
    {
        renderer_.set_draw_blend_mode(SDL_BLENDMODE_NONE);
        for (int col = 0; col < board_display_.grid_size.x; ++col) {
            for (int row = 0; row < board_display_.grid_size.y; ++row) {
                renderer_.set_draw_color(((row + col) % 2 == 0) ? pallete::white : pallete::jasons_dumbass_blue);
                renderer_.fill_rectangle(board_display_.grid_cell({row, col}));
            }
        }

        renderer_.set_draw_blend_mode(SDL_BLENDMODE_BLEND);
        const auto lock = std::lock_guard{pieces_mutex_};
        if (selected_piece_coordinate_.has_value() && selected_piece_valid_moves_.has_value()) {
            auto selected_color = pallete::light_green;
            selected_color.a = 0x7F;
            renderer_.set_draw_color(selected_color);
            renderer_.fill_rectangle(board_display_.grid_cell(transform_chess_to_grid_view(*selected_piece_coordinate_))
            );
            for (const auto move : selected_piece_valid_moves_->to_position_vector()) {
                const auto grid_cell = board_display_.grid_cell(transform_chess_to_grid_view(move));
                renderer_.fill_rectangle(grid_cell);
            }
        }
    }

    void render_pieces()
    {
        for (int col = 0; col < board_display_.grid_size.x; ++col) {
            for (int row = 0; row < board_display_.grid_size.y; ++row) {
                const auto coord = Position{row, col};

                const auto piece = pieces_.piece_at(coord);
                if (!piece.has_value()) {
                    continue;
                }
                const auto piece_rect = pieces_sprite_map_.get_region(*piece);
                const auto piece_position = board_display_.grid_cell_position(transform_chess_to_grid_view(coord));
                const auto piece_size = board_display_.cell_size();
                const auto screen_rect =
                    sdl::Rectangle<int>{piece_position.x, piece_position.y, piece_size.x, piece_size.y};
                renderer_.copy<int>(pieces_sprites_, piece_rect, screen_rect);
            }
        }
    }

    void render()
    {
        renderer_.set_draw_color(pallete::white);
        renderer_.clear();

        render_board();
        render_pieces();

        renderer_.present();
    }

    static constexpr sdl::Rectangle<int> screen_region{0, 0, 480, 480};
    static constexpr int max_frame_rate_per_second = 100;
    static constexpr auto min_frame_period_ms = std::chrono::milliseconds(1'000) / max_frame_rate_per_second;
    static constexpr auto window_config = sdl::WindowConfig{
        .title = "SDL Application",
        .x_position = SDL_WINDOWPOS_UNDEFINED,
        .y_position = SDL_WINDOWPOS_UNDEFINED,
        .width = screen_region.w,
        .height = screen_region.h,
        .flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE};
    static constexpr auto renderer_config =
        sdl::RendererConfig{.index = -1, .flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC};

    std::atomic_bool running_{true};
    MinimumPeriodWait<std::chrono::milliseconds> minimum_frame_delay_{std::chrono::milliseconds{min_frame_period_ms}};

    sdl::Window window_{window_config};
    sdl::Renderer renderer_{window_.get_pointer(), renderer_config};

    static constexpr auto board_size = 8;
    GridView board_display_{{board_size, board_size}, {screen_region.w, screen_region.h}};

    sdl::Texture pieces_sprites_{sdl::Texture{
        renderer_.make_texture_from_surface(sdl::image::load_image("resources/pieces_sprite_map.png").get())}};

    SpriteGrid<Piece> pieces_sprite_map_{
        pieces_sprites_.size(),
        Point{6, 2},
        {
            {{PieceColor::white, PieceType::king}, {0, 0}},
            {{PieceColor::white, PieceType::queen}, {1, 0}},
            {{PieceColor::white, PieceType::bishop}, {2, 0}},
            {{PieceColor::white, PieceType::knight}, {3, 0}},
            {{PieceColor::white, PieceType::rook}, {4, 0}},
            {{PieceColor::white, PieceType::pawn}, {5, 0}},
            {{PieceColor::black, PieceType::king}, {0, 1}},
            {{PieceColor::black, PieceType::queen}, {1, 1}},
            {{PieceColor::black, PieceType::bishop}, {2, 1}},
            {{PieceColor::black, PieceType::knight}, {3, 1}},
            {{PieceColor::black, PieceType::rook}, {4, 1}},
            {{PieceColor::black, PieceType::pawn}, {5, 1}},
        }};

    std::mutex pieces_mutex_;
    BoardPieces pieces_{BoardPieces::make_standard_setup_board()};
    std::optional<Position> selected_piece_coordinate_;
    std::optional<BitBoard> selected_piece_valid_moves_;

    EventHandlers<SDL_QuitEvent> quit_event_handlers_;
    EventHandlers<SDL_MouseButtonEvent> mouse_button_down_event_handlers_;
    EventHandlers<SDL_MouseButtonEvent> mouse_button_up_event_handlers_;
};

int main(int argc, char* argv[])
{
    spdlog::set_level(spdlog::level::debug);
    sdl::Context global_setup{sdl::InitFlags::Video};
    sdl::image::Context global_image_setup{sdl::image::InitFlags::png};
    ChessApplication{}.run();
    return 0;
}
