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

template <>
struct fmt::formatter<SDL_MouseButtonEvent> : fmt::formatter<std::string>
{
    auto format(SDL_MouseButtonEvent event, format_context& ctx) -> decltype(ctx.out())
    {
        return format_to(ctx.out(),
                         "[SDL_MouseButtonEvent type={}, timestamp={}, windowID={}, which={}, button={}, state={}, "
                         "clicks={}, padding1={}, x={}, y={}]",
                         event.type, event.timestamp, event.windowID, event.which, event.button, event.state,
                         event.clicks, event.padding1, event.x, event.y);
    }
};

template <>
struct fmt::formatter<sdl::Point<int>> : fmt::formatter<std::string>
{
    auto format(sdl::Point<int> point, format_context& ctx) -> decltype(ctx.out())
    {
        return format_to(ctx.out(), "[sdl::Point<int> x={}, y={}]", point.x, point.y);
    }
};

template <>
struct fmt::formatter<chess::Piece> : fmt::formatter<std::string>
{
    auto format(chess::Piece piece, format_context& ctx) -> decltype(ctx.out())
    {
        if (piece == chess::null_piece) {
            return format_to(ctx.out(), "null_piece");
        }
        return format_to(ctx.out(), "{} {}", chess::piece_color_names.at(piece.color), chess::piece_type_names.at(piece.type));
    }
};

template <typename Vec2>
sdl::Point<typename Vec2::dimension_type> vec2_to_sdl_point(Vec2 coordinate)
{
    return {coordinate.x(), coordinate.y()};
}

template <typename Rep, typename Period>
Rep to_milliseconds(std::chrono::duration<Rep, Period> duration)
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
}

template <typename Rectangle>
sdl::Point<sdl::rectangle_dimension_type<Rectangle>> rectangle_center(Rectangle rectangle)
{
    return {rectangle.x + rectangle.w / 2, rectangle.y + rectangle.h / 2};
}

namespace pallete {

constexpr sdl::Color black{0x00, 0x00, 0x00, 0xFF};
constexpr sdl::Color tan{0x0F, 0x0F, 0x00, 0xFF};
constexpr sdl::Color blue{0x00, 0x00, 0xFF, 0xFF};
constexpr sdl::Color jasons_dumbass_blue{129, 152, 201, 0xFF};
constexpr sdl::Color gray{0x0F, 0x0F, 0x0F, 0xFF};
constexpr sdl::Color white{0xFF, 0xFF, 0xFF, 0xFF};

} // namespace pallete

class SpriteMapGrid
{
  public:
    SpriteMapGrid(sdl::Point<int> map_size, sdl::Point<int> n_elements)
        : pitch_{map_size.x / n_elements.x, map_size.y / n_elements.y}
    {}

    [[nodiscard]] sdl::Rectangle<int> get_region(sdl::Point<int> coordinate) const;

  private:
    sdl::Point<int> pitch_;
};

sdl::Rectangle<int> SpriteMapGrid::get_region(sdl::Point<int> coordinate) const
{
    return {.x = coordinate.x * pitch_.x, .y = coordinate.y * pitch_.y, .w = pitch_.x, .h = pitch_.y};
}

template <typename T>
class SpriteGrid
{
  public:
    using CoordinateMap = std::map<T, sdl::Point<int>>;

    SpriteGrid(sdl::Point<int> texture_size, sdl::Point<int> grid_size, CoordinateMap&& sprite_coordinates)
        : pitch_{texture_size.x / grid_size.x, texture_size.y / grid_size.y},
          coordinates_{std::move(sprite_coordinates)}
    {}

    [[nodiscard]] sdl::Rectangle<int> get_region(const T& sprite);

  private:
    sdl::Point<int> pitch_;
    CoordinateMap coordinates_;

    [[nodiscard]] sdl::Rectangle<int> get_region(sdl::Point<int> coordinate) const;
};

template <typename T>
sdl::Rectangle<int> SpriteGrid<T>::get_region(sdl::Point<int> coordinate) const
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
        board_display_.set_on_cell_clicked_callback(
            [this](const sdl::Point<int>& point) { on_grid_cell_clicked(point); });
        process_events();
    }

    void on_grid_cell_clicked(const sdl::Point<int>& point)
    {
        spdlog::debug("clicked cell {}", point);
        const auto lock = std::lock_guard{pieces_mutex_};
        const auto coord = dm::Coord<int>{point.x, point.y};

        const auto current = pieces_.occupant_at(coord);
        spdlog::debug("cell contains piece: {}", current);
        PieceType next_type;
        PieceColor next_color = PieceColor::white;
        switch (current.type) {
        case PieceType::pawn:
            next_type = PieceType::knight;
            break;
        case PieceType::knight:
            next_type = PieceType::bishop;
            break;
        case PieceType::bishop:
            next_type = PieceType::rook;
            break;
        case PieceType::rook:
            next_type = PieceType::queen;
            break;
        case PieceType::queen:
            next_type = PieceType::king;
            break;
        case PieceType::king:
            next_type = PieceType::none;
            next_color = PieceColor::none;
            break;
        case PieceType::none:
            next_type = PieceType::pawn;
            break;
        default:
            assert(!"invalid piece");
            break;
        }
        pieces_.set_piece({next_color, next_type}, coord);
    }

    void initialize_event_handlers()
    {
        quit_event_handlers_.add_handler([this](const SDL_QuitEvent& event) { handle_quit_event(event); });
        mouse_button_down_event_handlers_.add_handler(
            [this](const SDL_MouseButtonEvent& event) { board_display_.on_button_down(event); });
        mouse_button_up_event_handlers_.add_handler(
            [this](const SDL_MouseButtonEvent& event) { board_display_.on_button_up(event); });
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
            case SDL_MOUSEMOTION:
                mouse_motion_event_handlers_.call_all(event->motion);
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
        for (int col = 0; col < board_display_.grid_size.x; ++col) {
            for (int row = 0; row < board_display_.grid_size.y; ++row) {
                renderer_.set_draw_color(((row + col) % 2 == 0) ? pallete::white : pallete::jasons_dumbass_blue);
                renderer_.fill_rectangle(board_display_.grid_element({col, row}));
            }
        }
    }

    void on_grid_view_cell_clicked(const GridView::Point& point) {}

    void render_pieces()
    {
        for (int col = 0; col < board_display_.grid_size.x; ++col) {
            for (int row = 0; row < board_display_.grid_size.y; ++row) {
                const auto coord = dm::Coord<int>{row, col};
                if (!pieces_.occupied(coord)) {
                    continue;
                }
                const auto piece_rect = pieces_sprite_map_.get_region(pieces_.occupant_at(coord));
                const auto piece_position = board_display_.grid_element_position(vec2_to_sdl_point(coord));
                const auto piece_size = board_display_.element_size();
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
    static constexpr auto window_config = sdl::WindowConfig{.title = "SDL Application",
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

    SpriteGrid<Piece> pieces_sprite_map_{pieces_sprites_.size(),
                                         sdl::Point<int>{6, 2},
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

    Pieces pieces_;
    std::mutex pieces_mutex_;

    EventHandlers<SDL_QuitEvent> quit_event_handlers_;
    EventHandlers<SDL_MouseMotionEvent> mouse_motion_event_handlers_;
    EventHandlers<SDL_MouseButtonEvent> mouse_button_down_event_handlers_;
    EventHandlers<SDL_MouseButtonEvent> mouse_button_up_event_handlers_;
};

int main(int argc, char* argv[])
{
    using namespace sdl;
    spdlog::set_level(spdlog::level::debug);
    Context global_setup{InitFlags::Video};
    image::Context global_image_setup{image::InitFlags::png};
    ChessApplication{}.run();
    return 0;
}
