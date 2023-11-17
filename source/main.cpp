#include "event_handlers.h"
#include "game.h"
#include "grid_view.h"
#include "pieces.h"
#include "sprite_map_grid.h"
#include "timing.h"

#include "vec2_formatter.h"

#include "sdlpp.h"
#include "sdlpp_image.h"

#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_sdlrenderer2.h"
#include "imgui.h"

#include <spdlog/cfg/env.h>
#include <spdlog/fmt/fmt.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include <gsl/gsl>

#include <chrono>
#include <exception>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <thread>

namespace chrono = std::chrono;
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
            "type={}, timestamp={}, windowID={}, which={}, button={}, "
            "state={}, clicks={}, padding1={}, x={}, y={}]",
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

inline ImVec2 make_im_vec2(int x, int y)
{
    return ImVec2{gsl::narrow_cast<float>(x), gsl::narrow_cast<float>(y)};
}

inline ImVec2 make_im_vec2(sdl::Point<int> point)
{
    return make_im_vec2(point.x, point.y);
}

inline sdl::Point<int> make_point(float x, float y)
{
    return sdl::Point<int>{gsl::narrow_cast<int>(std::ceil(x)), gsl::narrow_cast<int>(std::ceil(y))};
}

inline sdl::Point<int> make_point(ImVec2 im_vec2)
{
    return make_point(im_vec2.x, im_vec2.y);
}

Point transform_chess_to_grid_view(Position coordinate)
{
    return {.x = coordinate.y(), .y = coordinate.x()};
}

Position transform_grid_view_to_chess(Point coordinate)
{
    return {coordinate.y, coordinate.x};
}

template <sdl::RectangleT Rectangle>
sdl::Point<sdl::rectangle_dimension_type<Rectangle>> rectangle_center(const Rectangle rectangle)
{
    return {rectangle.x + rectangle.w / 2, rectangle.y + rectangle.h / 2};
}

template <sdl::RectangleT Rectangle>
sdl::Point<float> rectangle_center_f(const Rectangle rectangle)
{
    return {rectangle.x + rectangle.w / 2.0F, rectangle.y + rectangle.h / 2.0F};
}

template <typename Rectangle>
sdl::Point<sdl::rectangle_dimension_type<Rectangle>> rectangle_origin(const Rectangle rectangle)
{
    return {rectangle.x, rectangle.y};
}

template <typename Rectangle>
sdl::rectangle_dimension_type<Rectangle> rectangle_area(const Rectangle rectangle)
{
    return rectangle.w * rectangle.h;
}

template <typename Point>
sdl::point_dimension_type<Point> size_area(Point size)
{
    return size.x * size.y;
}

namespace pallete {

[[maybe_unused]] static constexpr sdl::Color black{0x00, 0x00, 0x00, 0xFF};
[[maybe_unused]] static constexpr sdl::Color gray{0x0F, 0x0F, 0x0F, 0xFF};
[[maybe_unused]] static constexpr sdl::Color blue{0x00, 0x00, 0xFF, 0xFF};
[[maybe_unused]] static constexpr sdl::Color jasons_dumbass_blue{129, 152, 201, 0xFF};
[[maybe_unused]] static constexpr sdl::Color light_green{169, 216, 145, 0xFF};
[[maybe_unused]] static constexpr sdl::Color light_red{214, 32, 35, 0xFF};
[[maybe_unused]] static constexpr sdl::Color light_purple{175, 41, 198, 0xFF};
[[maybe_unused]] static constexpr sdl::Color light_gray{0xF8, 0xF8, 0xF8, 0xFF};
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
    ChessApplication(sdl::Window&& window, sdl::Renderer&& renderer)
        : window_{std::move(window)},
          renderer_{std::move(renderer)},
          board_display_{
              {board_size, board_size},
              {0, 0, board_display_texture_properties_.width, board_display_texture_properties_.height}},
          pieces_sprite_map_{
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
              }}
    {
        IMGUI_CHECKVERSION();
        initialize_event_handlers();
        board_display_.set_on_cell_clicked_callback([this](const Point& point) { on_grid_cell_clicked(point); });

        auto pieces_image = sdl::image::load_sized_svg(sprite_map_filename, {1000, 0});
        pieces_sprite_map_.texture() = sdl::Texture{renderer_.make_texture_from_surface(pieces_image.get())};

        for (const auto color : {PieceColor::black, PieceColor::white}) {
            for (const auto type : {PieceType::knight, PieceType::bishop, PieceType::rook, PieceType::queen}) {
                const auto piece = Piece{color, type};
                piece_textures_.insert({piece, make_piece_texture(piece, {100, 100})});
            }
        }
    }

    sdl::Texture make_piece_texture(const Piece piece, const sdl::Point<int> size)
    {
        sdl::Texture::Properties texture_properties = {
            SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, size.x, size.y};
        sdl::Texture texture = renderer_.make_texture(texture_properties);

        SDL_Texture* original_target = renderer_.get_render_target();
        auto restore_target = gsl::finally([this, original_target] { renderer_.set_render_target(original_target); });

        renderer_.set_render_target(texture.get_pointer());
        renderer_.set_draw_color(pallete::white);
        renderer_.clear();
        renderer_.copy(
            pieces_sprite_map_.texture(), pieces_sprite_map_.get_region(piece), sdl::make_rectangle({0, 0}, size)
        );

        return texture;
    }

    void run()
    {
        while (running_) {
            process_events();
            update_board();
            if (window_.shown()) {
                new_frame();
                render_frame();
            }
            spdlog::default_logger()->flush();
            minimum_period_delay_.wait_until_done_and_restart();
        }
    }

  private:
    void update_board()
    {
        const auto move = move_selection_.load();
        if (move.has_value() && !selecting_promotion_) {
            pieces_.make_move(*move, promotion_selection_);
            move_selection_ = std::nullopt;
            promotion_selection_ = std::nullopt;
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

        key_down_event_handlers_.add_handler([this](const SDL_KeyboardEvent& event) {
            if (event.keysym.sym == SDLK_a && event.repeat == 0) {
                highlight_attacked_ = !highlight_attacked_;
            }
        });
    }

    void new_frame()
    {
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        renderer_.set_scale(ImGui::GetIO().DisplayFramebufferScale.x, ImGui::GetIO().DisplayFramebufferScale.y);
        renderer_.set_draw_color(pallete::white);
        renderer_.clear();
    }

    void render_frame()
    {
        using namespace sdl::point_operators;

        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
        ImGui::ShowDemoWindow(&show_demo_window_);

        show_menu();

        ImGui::Begin("Game Window");

        const auto board_display_origin =
            make_point(ImGui::GetWindowPos()) + make_point(ImGui::GetWindowContentRegionMin());
        const auto board_display_size = make_point(ImGui::GetContentRegionAvail());
        const auto board_display_rectangle = sdl::make_rectangle(board_display_origin, board_display_size);
        update_board_display_region(board_display_rectangle);

        if (selecting_promotion_) {
            ImGui::OpenPopup("Promotion");
        }
        const auto promotion_window_flags =
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove;

        const auto board_center = rectangle_center_f(board_display_rectangle);
        ImGui::SetNextWindowPos(ImVec2(board_center.x, board_center.y), 0, ImVec2(0.5F, 0.5F));
        if (ImGui::BeginPopupModal("Promotion", nullptr, promotion_window_flags)) {
            for (const auto piece_type : promotion_piece_types_) {
                const auto piece = Piece{pieces_.active_color(), piece_type};
                const auto& texture = piece_textures_.at(piece);

                auto button_size = ImVec2(50.F, 50.F);
                auto bg_col = ImVec4(0.F, 0.F, 0.F, 1.F);
                auto tint_col = ImVec4(1.F, 1.F, 1.F, 1.F);
                if (ImGui::ImageButton(
                        piece.to_string().c_str(),
                        texture.get_pointer(),
                        button_size,
                        ImVec2(0.F, 0.F),
                        ImVec2(1.F, 1.F),
                        bg_col,
                        tint_col
                    )) {
                    promotion_selection_ = piece_type;
                    selecting_promotion_ = false;
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine();
            }
            ImGui::EndPopup();
        }

        ImGui::Image(board_display_.texture().get_pointer(), make_im_vec2(board_display_.texture().size()));
        ImGui::End();

        render_game();

        ImGui::Render();
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());

        renderer_.present();
    }

    void show_menu()
    {
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("Menu")) {
                if (ImGui::MenuItem("New Game")) {
                    pieces_ = GameBoard{};
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }
    }

    void render_game()
    {
        SDL_Texture* original_target = renderer_.get_render_target();
        auto restore_target = gsl::finally([this, original_target] { renderer_.set_render_target(original_target); });
        renderer_.set_render_target(board_display_.texture().get_pointer());

        render_board();
        render_pieces();
    }

    void update_board_display_region(const sdl::Rectangle<int> region)
    {
        using namespace sdl::point_operators;

        if (rectangle_area(region) <= 0) {
            return;
        }

        board_display_.set_origin(rectangle_origin(region));

        auto min_length = (std::min(region.w, region.h) / board_size) * board_size;
        const auto new_size = ::Point{min_length, min_length};
        if (board_display_.size() == new_size) {
            return;
        }

        board_display_.set_size(new_size);
        board_display_texture_properties_.set_size(new_size);
        board_display_.texture() = sdl::Texture{renderer_.make_texture(board_display_texture_properties_)};

        const auto sprite_map_texture_size = sdl::Point<int>{std::max(board_display_.region().w, 100), 0};
        auto pieces_image = sdl::image::load_sized_svg(sprite_map_filename, sprite_map_texture_size);
        pieces_sprite_map_.texture() = sdl::Texture{renderer_.make_texture_from_surface(pieces_image.get())};
    }

    void render_board()
    {
        using namespace sdl;
        renderer_.set_draw_blend_mode(SDL_BLENDMODE_NONE);
        for (int col = 0; col < board_display_.grid_size.x; ++col) {
            for (int row = 0; row < board_display_.grid_size.y; ++row) {
                renderer_.set_draw_color(((row + col) % 2 == 0) ? pallete::white : pallete::jasons_dumbass_blue);
                renderer_.fill_rectangle(board_display_.grid_cell_local({row, col}));
            }
        }

        renderer_.set_draw_blend_mode(SDL_BLENDMODE_BLEND);
        const auto lock = std::lock_guard{pieces_mutex_};
        if (selected_piece_coordinate_.has_value()) {
            renderer_.set_draw_color(pallete::color_with_alpha(pallete::light_green, 0x7F));
            renderer_.fill_rectangle(
                board_display_.grid_cell_local(transform_chess_to_grid_view(*selected_piece_coordinate_))
            );
            for (const auto move : selected_piece_valid_moves_) {
                renderer_.fill_rectangle(board_display_.grid_cell_local(transform_chess_to_grid_view(move)));
            }
        }

        if (pieces_.is_in_checkmate()) {
            renderer_.set_draw_color(pallete::color_with_alpha(pallete::black, 0x7F));
            renderer_.fill_rectangle(
                board_display_.grid_cell_local(transform_chess_to_grid_view(pieces_.active_king_position()))
            );
        } else if (pieces_.is_active_in_check()) {
            renderer_.set_draw_color(pallete::color_with_alpha(pallete::light_red, 0x7F));
            renderer_.fill_rectangle(
                board_display_.grid_cell_local(transform_chess_to_grid_view(pieces_.active_king_position()))
            );
        } else if (pieces_.is_in_stalemate()) {
            renderer_.set_draw_color(pallete::color_with_alpha(pallete::light_purple, 0x7F));
            renderer_.fill_rectangle(
                board_display_.grid_cell_local(transform_chess_to_grid_view(pieces_.active_king_position()))
            );
        }

        if (highlight_attacked_) {
            for (const auto attacked_position : pieces_.attacked_by_vector<PieceColor::black>()) {
                renderer_.set_draw_color(pallete::color_with_alpha(pallete::light_purple, 0x7F));
                renderer_.fill_rectangle(board_display_.grid_cell_local(transform_chess_to_grid_view(attacked_position))
                );
            }

            for (const auto attacked_position : pieces_.attacked_by_vector<PieceColor::white>()) {
                renderer_.set_draw_color(pallete::color_with_alpha(pallete::light_red, 0x7F));
                renderer_.fill_rectangle(board_display_.grid_cell_local(transform_chess_to_grid_view(attacked_position))
                );
            }
        }
    }

    void render_pieces()
    {
        renderer_.set_draw_blend_mode(SDL_BLENDMODE_NONE);
        for (int col = 0; col < board_display_.grid_size.x; ++col) {
            for (int row = 0; row < board_display_.grid_size.y; ++row) {
                const auto coord = Position{row, col};

                const auto piece = pieces_.piece_at(coord);
                if (!piece.has_value()) {
                    continue;
                }
                const auto piece_position =
                    board_display_.grid_cell_position_local(transform_chess_to_grid_view(coord));
                const auto piece_size = board_display_.cell_size();
                const auto screen_rect = sdl::make_rectangle(piece_position, piece_size);
                renderer_.copy(pieces_sprite_map_.texture(), pieces_sprite_map_.get_region(*piece), screen_rect);
            }
        }
    }

    void on_grid_cell_clicked(const Point& point)
    {
        const auto lock = std::lock_guard{pieces_mutex_};
        const auto coord = transform_grid_view_to_chess(point);
        if (selected_piece_coordinate_.has_value()) {
            if (!selected_piece_valid_moves_.contains(coord)) {
                spdlog::debug("invalid move");
            } else {
                const auto move = GameBoard::Move{*selected_piece_coordinate_, coord};
                move_selection_ = move;
                // selecting_promotion_ = pieces_.is_promotion_move(move);
                selecting_promotion_ = true;
            }
            selected_piece_coordinate_ = std::nullopt;
            selected_piece_valid_moves_.clear();
        } else {
            if (pieces_.is_active_piece(coord)) {
                selected_piece_coordinate_ = std::optional{coord};
                selected_piece_valid_moves_ = pieces_.valid_moves_set(coord);
            }
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
            if (!event.has_value()) {
                throw std::runtime_error("empty optional event"); // to get clangd to be quiet
            }
            ImGui_ImplSDL2_ProcessEvent(&*event);
            switch (event->type) {
            case SDL_QUIT:
                quit_event_handlers_.call_all(event->quit);
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

    void handle_quit_event([[maybe_unused]] const SDL_QuitEvent& event)
    {
        running_ = false;
    }

    std::atomic_bool running_{true};

    bool show_demo_window_ = true;

    using fp_milliseconds = chrono::duration<float, chrono::milliseconds::period>;
    static constexpr int max_frames_per_second = 60;
    static constexpr auto period_duration = fp_milliseconds{1000.0F / max_frames_per_second};
    Timer minimum_period_delay_{period_duration};

    sdl::Window window_;
    sdl::Renderer renderer_;

    static constexpr auto board_size = 8;

    sdl::Texture::Properties board_display_texture_properties_ = {
        SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, 1000, 1000};
    ClickableGrid board_display_;

    static constexpr const char* sprite_map_filename = "resources/pieces_sprite_map.svg";
    SpriteGrid<Piece> pieces_sprite_map_;
    std::map<Piece, sdl::Texture> piece_textures_;
    std::array<PieceType, 4> promotion_piece_types_{
        PieceType::knight, PieceType::bishop, PieceType::rook, PieceType::queen};

    std::mutex pieces_mutex_;
    GameBoard pieces_;
    std::optional<Position> selected_piece_coordinate_;
    std::set<Position> selected_piece_valid_moves_;
    std::atomic<std::optional<GameBoard::Move>> move_selection_;
    std::optional<PieceType> promotion_selection_;
    std::atomic_bool selecting_promotion_{false};
    std::atomic_bool highlight_attacked_{false};

    EventHandlers<SDL_QuitEvent> quit_event_handlers_;
    EventHandlers<SDL_MouseButtonEvent> mouse_button_down_event_handlers_;
    EventHandlers<SDL_MouseButtonEvent> mouse_button_up_event_handlers_;
    EventHandlers<SDL_WindowEvent> window_resize_handlers_;
    EventHandlers<SDL_KeyboardEvent> key_down_event_handlers_;
    EventHandlers<SDL_KeyboardEvent> key_up_event_handlers_;
};

void init_logging()
{
    spdlog::cfg::load_env_levels();

    auto logger = spdlog::basic_logger_mt("chess_logger", "log.txt");
    spdlog::set_default_logger(logger);
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
    init_logging();

    sdl::initialize(sdl::InitFlags::video | sdl::InitFlags::events);
    auto sdl_cleanup = gsl::finally([] { sdl::quit(); });
    sdl::image::initialize(sdl::image::InitFlags::png);
    auto sdl_image_cleanup = gsl::finally([] { sdl::image::quit(); });

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

    static constexpr auto window_config = sdl::WindowConfig{
        .title = "Chess",
        .x_position = SDL_WINDOWPOS_UNDEFINED,
        .y_position = SDL_WINDOWPOS_UNDEFINED,
        .width = 800,
        .height = 600,
        .flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE};
    static constexpr auto renderer_config =
        sdl::RendererConfig{.index = -1, .flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC};

    auto window = sdl::Window(window_config);
    auto renderer = sdl::Renderer(window.get_pointer(), renderer_config);

    ImGui::CreateContext();
    auto imgui_context_cleanup = gsl::finally([] { ImGui::DestroyContext(); });

    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForSDLRenderer(window.get_pointer(), renderer.get_pointer());
    auto imgui_sdl2_shutdown = gsl::finally([] { ImGui_ImplSDL2_Shutdown(); });
    ImGui_ImplSDLRenderer2_Init(renderer.get_pointer());
    auto imgui_sdl2_renderer_shutdown = gsl::finally([] { ImGui_ImplSDLRenderer2_Shutdown(); });

    ChessApplication{std::move(window), std::move(renderer)}.run();
    return EXIT_SUCCESS;
}
