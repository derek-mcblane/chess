#include "board.h"
#include "event_handlers.h"
#include "grid_view.h"
#include "pieces.h"
#include "sprite_map_grid.h"
#include "timing.h"

#include <gsl/gsl>

#include "vec2_formatter.h"
#include <spdlog/cfg/env.h>
#include <spdlog/spdlog.h>

#include "sdlpp.h"
#include "sdlpp_image.h"

#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_sdlrenderer2.h"
#include "imgui.h"

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
        return fmt::format_to(
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
        return fmt::format_to(ctx.out(), "[Point x={}, y={}]", point.x, point.y);
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
[[maybe_unused]] static constexpr sdl::Color light_red{214, 32, 35, 0xFF};
[[maybe_unused]] static constexpr sdl::Color light_purple{175, 41, 198, 0xFF};
[[maybe_unused]] static constexpr sdl::Color gray{0x0F, 0x0F, 0x0F, 0xFF};
[[maybe_unused]] static constexpr sdl::Color white{0xFF, 0xFF, 0xFF, 0xFF};

[[nodiscard]] sdl::Color color_with_alpha(sdl::Color color, Uint8 alpha)
{
    color.a = alpha;
    return color;
}

} // namespace pallete

class ChessApplication
{
  public:
    ChessApplication(std::unique_ptr<sdl::Window> window, std::unique_ptr<sdl::Renderer> renderer)
        : window_{std::move(window)},
          renderer_{std::move(renderer)},
          board_display_{{board_size, board_size}, {0, 0, window_->width(), window_->height()}}
    {
        IMGUI_CHECKVERSION();
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
            if (!selected_piece_valid_moves_->contains(coord)) {
                spdlog::debug("invalid move");
            } else {
                spdlog::debug("moving from {} to {}", *selected_piece_coordinate_, coord);
                const auto move = Board::Move{*selected_piece_coordinate_, coord};
                if (pieces_.is_promotion_move(move)) {
                    const std::optional<PieceType> promotion_piece = PieceType::queen;
                    pieces_.promote({move, promotion_piece});
                } else {
                    pieces_.make_move(move);
                }
            }
            selected_piece_coordinate_ = std::nullopt;
            selected_piece_valid_moves_ = std::nullopt;
        } else {
            if (pieces_.is_active_piece(coord)) {
                selected_piece_coordinate_ = std::optional{coord};
                selected_piece_valid_moves_ = pieces_.valid_moves_set(coord);
            }
        }
    }

    void initialize_event_handlers()
    {
        quit_event_handlers_.add_handler([this](const SDL_QuitEvent& event) { handle_quit_event(event); });
        window_resize_handlers_.add_handler([this](const SDL_WindowEvent& event) {
            using namespace sdl;
            const auto window_size = ::Point{event.data1, event.data2};
            const auto min_dimension_size = std::min(window_size.x, window_size.y);

            const auto board_origin = (window_size - board_display_.size()) / 2;
            const auto board_size = ::Point{min_dimension_size, min_dimension_size};
            board_display_.region() = {board_origin.x, board_origin.y, board_size.x, board_size.y};
        });

        mouse_button_down_event_handlers_.add_handler([this](const SDL_MouseButtonEvent& event) {
            board_display_.on_button_down(event);
        });
        mouse_button_up_event_handlers_.add_handler([this](const SDL_MouseButtonEvent& event) {
            board_display_.on_button_up(event);
        });

        key_down_event_handlers_.add_handler([this](const SDL_KeyboardEvent& event) {
            if (event.keysym.sym == SDLK_a && event.repeat == 0) {
                highlight_attacked_ = !highlight_attacked_;
            }
        });
    }

    void run()
    {
        while (running_) {
            begin_frame();
            update();
            if (window_->shown()) {
                render();
            }
            process_events();
            end_frame();
        }
    }

  private:
    void begin_frame() {}

    void end_frame()
    {
        minimum_frame_delay_.end_interval();
        constexpr bool log_frame_duration = false;
        if (log_frame_duration) {
            const auto frame_duration = minimum_frame_delay_.previous_interval_duration();
            spdlog::debug("{} ms", to_milliseconds(frame_duration));
        }
    }

    void handle_window_events(const SDL_WindowEvent& event)
    {
        switch (event.event) {
        case SDL_WINDOWEVENT_RESIZED:
            spdlog::debug("SDL_WINDOWEVENT_RESIZED");
            window_resize_handlers_.call_all(event);
            break;
        case SDL_WINDOWEVENT_SIZE_CHANGED:
            spdlog::debug("SDL_WINDOWEVENT_SIZE_CHANGED");
            window_resize_handlers_.call_all(event);
            break;
        }
    }

    void handle_keyboard_events(const SDL_KeyboardEvent& event)
    {
        switch (event.type) {
        case SDL_KEYDOWN:
            key_down_event_handlers_.call_all(event);
            break;
        case SDL_KEYUP:
            key_up_event_handlers_.call_all(event);
            break;
        }
    }

    void process_events()
    {
        while (const auto event = sdl::poll_event()) {
            ImGui_ImplSDL2_ProcessEvent(&*event);
            switch (event->type) {
            case SDL_QUIT:
                quit_event_handlers_.call_all(event->quit);
                break;
            case SDL_WINDOWEVENT:
                handle_window_events(event->window);
                break;
            case SDL_MOUSEBUTTONDOWN:
                spdlog::debug(
                    "[SDL_MOUSEBUTTONDOWN button={}, position={}",
                    event->button.button,
                    Point{event->button.x, event->button.y}
                );
                mouse_button_down_event_handlers_.call_all(event->button);
                break;
            case SDL_MOUSEBUTTONUP:
                spdlog::debug(
                    "[SDL_MOUSEBUTTONUP button={}, position={}",
                    event->button.button,
                    Point{event->button.x, event->button.y}
                );
                mouse_button_up_event_handlers_.call_all(event->button);
                break;
            case SDL_KEYDOWN:
            case SDL_KEYUP:
                handle_keyboard_events(event->key);
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

    void update() {}

    void render_board()
    {
        using namespace sdl;
        renderer_->set_draw_blend_mode(SDL_BLENDMODE_NONE);
        for (int col = 0; col < board_display_.grid_size.x; ++col) {
            for (int row = 0; row < board_display_.grid_size.y; ++row) {
                renderer_->set_draw_color(((row + col) % 2 == 0) ? pallete::white : pallete::jasons_dumbass_blue);
                renderer_->fill_rectangle(board_display_.grid_cell({row, col}));
            }
        }

        renderer_->set_draw_blend_mode(SDL_BLENDMODE_BLEND);
        const auto lock = std::lock_guard{pieces_mutex_};
        if (selected_piece_coordinate_.has_value() && selected_piece_valid_moves_.has_value()) {
            renderer_->set_draw_color(pallete::color_with_alpha(pallete::light_green, 0x7F));
            renderer_->fill_rectangle(board_display_.grid_cell(transform_chess_to_grid_view(*selected_piece_coordinate_)
            ));
            for (const auto move : *selected_piece_valid_moves_) {
                renderer_->fill_rectangle(board_display_.grid_cell(transform_chess_to_grid_view(move)));
            }
        }

        if (highlight_attacked_) {
            for (const auto attacked_position : pieces_.attacked_by_black()) {
                renderer_->set_draw_color(pallete::color_with_alpha(pallete::light_purple, 0x7F));
                renderer_->fill_rectangle(board_display_.grid_cell(transform_chess_to_grid_view(attacked_position)));
            }

            for (const auto attacked_position : pieces_.attacked_by_white()) {
                renderer_->set_draw_color(pallete::color_with_alpha(pallete::light_red, 0x7F));
                renderer_->fill_rectangle(board_display_.grid_cell(transform_chess_to_grid_view(attacked_position)));
            }
        }
    }

    void render_pieces()
    {
        renderer_->set_draw_blend_mode(SDL_BLENDMODE_NONE);
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
                renderer_->copy<int>(pieces_sprites_, piece_rect, screen_rect);
            }
        }
    }

    void render()
    {
        ImGuiIO& io = ImGui::GetIO();
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        ImGui::Render();

        renderer_->set_scale(io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
        renderer_->set_draw_color(pallete::white);
        renderer_->clear();

        render_board();
        render_pieces();

        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
        renderer_->present();
    }

    static constexpr int max_frames_per_second = 60;
    static constexpr auto min_frame_period_ms = std::chrono::milliseconds(1'000) / max_frames_per_second;
    std::atomic_bool running_{true};
    MinimumPeriodWait<std::chrono::milliseconds> minimum_frame_delay_{std::chrono::milliseconds{min_frame_period_ms}};

    std::unique_ptr<sdl::Window> window_;
    std::unique_ptr<sdl::Renderer> renderer_;

    static constexpr auto board_size = 8;
    ClickableGrid board_display_;

    sdl::Texture pieces_sprites_{sdl::Texture{
        renderer_->make_texture_from_surface(sdl::image::load_image("resources/pieces_sprite_map.png").get())}};

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
    Board pieces_{Board::make_standard_setup_board()};
    std::optional<Position> selected_piece_coordinate_;
    std::optional<std::set<Position>> selected_piece_valid_moves_;
    std::atomic_bool highlight_attacked_{false};

    EventHandlers<SDL_QuitEvent> quit_event_handlers_;
    EventHandlers<SDL_MouseButtonEvent> mouse_button_down_event_handlers_;
    EventHandlers<SDL_MouseButtonEvent> mouse_button_up_event_handlers_;
    EventHandlers<SDL_WindowEvent> window_resize_handlers_;
    EventHandlers<SDL_KeyboardEvent> key_down_event_handlers_;
    EventHandlers<SDL_KeyboardEvent> key_up_event_handlers_;
};

int main(int argc, char* argv[])
{
    spdlog::cfg::load_env_levels();

    sdl::initialize(sdl::InitFlags::Video);
    auto sdl_cleanup = gsl::finally([] { sdl::quit(); });
    sdl::image::initialize(sdl::image::InitFlags::png);
    auto sdl_image_cleanup = gsl::finally([] { sdl::image::quit(); });

    // SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

    static constexpr auto window_config = sdl::WindowConfig{
        .title = "Chess",
        .x_position = SDL_WINDOWPOS_UNDEFINED,
        .y_position = SDL_WINDOWPOS_UNDEFINED,
        .width = 480,
        .height = 480,
        .flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE};
    static constexpr auto renderer_config =
        sdl::RendererConfig{.index = -1, .flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC};

    auto window = std::make_unique<sdl::Window>(window_config);
    auto renderer = std::make_unique<sdl::Renderer>(window->get_pointer(), renderer_config);

    ImGui::CreateContext();
    auto imgui_context_cleanup = gsl::finally([] { ImGui::DestroyContext(); });

    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForSDLRenderer(window->get_pointer(), renderer->get_pointer());
    auto imgui_sdl2_shutdown = gsl::finally([] { ImGui_ImplSDL2_Shutdown(); });
    ImGui_ImplSDLRenderer2_Init(renderer->get_pointer());
    auto imgui_sdl2_renderer_shutdown = gsl::finally([] { ImGui_ImplSDLRenderer2_Shutdown(); });

    ChessApplication{std::move(window), std::move(renderer)}.run();
    return 0;
}
