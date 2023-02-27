#include "sdlpp.h"
#include "sdlpp_image.h"
#include "timing.h"

#include <SDL_render.h>
#include <gsl/gsl>

#include <chrono>
#include <exception>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

template <typename Rep, typename Period>
Rep to_milliseconds(std::chrono::duration<Rep, Period> duration)
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
}

namespace sdl::pallete {

constexpr Color black{0x00, 0x00, 0x00, 0xFF};
constexpr Color tan{0x0F, 0x0F, 0x00, 0xFF};
constexpr Color blue{0x00, 0x00, 0xFF, 0xFF};
constexpr Color gray{0x0F, 0x0F, 0x0F, 0xFF};
constexpr Color white{0xFF, 0xFF, 0xFF, 0xFF};

} // namespace sdl::pallete

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

enum class Colors : int
{
    black,
    white,
    n_colors_,
};
static constexpr size_t n_colors = static_cast<int>(Colors::n_colors_);

enum class Pieces
{
    pawn,
    knight,
    bishop,
    rook,
    queen,
    king,
    n_pieces_,
};
static constexpr size_t n_pieces = static_cast<int>(Pieces::n_pieces_);

void handle_window_event(const SDL_Event* event)
{
    SDL_WindowEvent window_event = event->window;
    switch (event->window.event) {
    case SDL_WINDOWEVENT_SHOWN:
        std::cout << "Window " << window_event.windowID << " event: ";
        std::cout << "shown";
        std::cout << '\n';
        break;
    case SDL_WINDOWEVENT_HIDDEN:
        std::cout << "Window " << window_event.windowID << " event: ";
        std::cout << "hidden";
        std::cout << '\n';
        break;
    case SDL_WINDOWEVENT_EXPOSED:
        std::cout << "Window " << window_event.windowID << " event: ";
        std::cout << "exposed";
        std::cout << '\n';
        break;
    case SDL_WINDOWEVENT_MOVED:
        std::cout << "Window " << window_event.windowID << " event: ";
        std::cout << "moved";
        std::cout << '\n';
        break;
    case SDL_WINDOWEVENT_RESIZED:
        std::cout << "Window " << window_event.windowID << " event: ";
        std::cout << "resized";
        std::cout << '\n';
        break;
    case SDL_WINDOWEVENT_SIZE_CHANGED:
        std::cout << "Window " << window_event.windowID << " event: ";
        std::cout << "size changed";
        std::cout << '\n';
        break;
    case SDL_WINDOWEVENT_MINIMIZED:
        std::cout << "Window " << window_event.windowID << " event: ";
        std::cout << "minimized";
        std::cout << '\n';
        break;
    case SDL_WINDOWEVENT_MAXIMIZED:
        std::cout << "Window " << window_event.windowID << " event: ";
        std::cout << "maximized";
        std::cout << '\n';
        break;
    case SDL_WINDOWEVENT_RESTORED:
        std::cout << "Window " << window_event.windowID << " event: ";
        std::cout << "restored";
        std::cout << '\n';
        break;
    case SDL_WINDOWEVENT_ENTER:
        std::cout << "Window " << window_event.windowID << " event: ";
        std::cout << "enter";
        std::cout << '\n';
        break;
    case SDL_WINDOWEVENT_LEAVE:
        std::cout << "Window " << window_event.windowID << " event: ";
        std::cout << "leave";
        std::cout << '\n';
        break;
    case SDL_WINDOWEVENT_FOCUS_GAINED:
        std::cout << "Window " << window_event.windowID << " event: ";
        std::cout << "focuse gained";
        std::cout << '\n';
        break;
    case SDL_WINDOWEVENT_FOCUS_LOST:
        std::cout << "Window " << window_event.windowID << " event: ";
        std::cout << "focus lost";
        std::cout << '\n';
        break;
    case SDL_WINDOWEVENT_CLOSE:
        std::cout << "Window " << window_event.windowID << " event: ";
        std::cout << "close";
        std::cout << '\n';
        break;
#if SDL_VERSION_ATLEAST(2, 0, 5)
    case SDL_WINDOWEVENT_TAKE_FOCUS:
        std::cout << "Window " << window_event.windowID << " event: ";
        std::cout << "take focus";
        std::cout << '\n';
        break;
    case SDL_WINDOWEVENT_HIT_TEST:
        std::cout << "Window " << window_event.windowID << " event: ";
        std::cout << "hit test";
        std::cout << '\n';
        break;
#endif
    default:
        break;
    }
}

void handle_quit_event(SDL_Event *event)
{

}

int window_event_callback(void *userdata, SDL_Event *event)
{
    handle_window_event(event);
    return 1;
}

int main(int argc, char* argv[])
{
    sdl::Application application{SDL_INIT_VIDEO};
    sdl::image::Extensions extensions{IMG_INIT_PNG};

    sdl::set_event_filter(window_event_callback, nullptr);

    constexpr sdl::Rectangle<int> screen_region{0, 0, 640, 480};
    constexpr int max_frame_rate_per_second = 100;
    constexpr auto min_frame_period_ms = std::chrono::milliseconds(1'000) / max_frame_rate_per_second;

    constexpr auto window_config = sdl::WindowConfig{.title = "SDL Application",
                                                     .x_position = SDL_WINDOWPOS_UNDEFINED,
                                                     .y_position = SDL_WINDOWPOS_UNDEFINED,
                                                     .width = screen_region.w,
                                                     .height = screen_region.h,
                                                     .flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE};

    constexpr auto renderer_config =
        sdl::RendererConfig{.index = -1, .flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC};

    auto window = sdl::Window{window_config};
    auto renderer = sdl::Renderer{window.get_pointer(), renderer_config};

    auto piece_sprites = sdl::Texture{
        renderer.make_texture_from_surface(sdl::image::load_image("resources/pieces_sprite_map.png").get())};
    SpriteMapGrid piece_sprites_grid{piece_sprites.size(), {n_pieces, n_colors}};

    bool running{true};
    MinimumPeriodWait minimum_frame_delay{std::chrono::milliseconds{min_frame_period_ms}};
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event) != 0) {
            switch (event.type) {
            case SDL_QUIT:
                running = false;
                break;
            case SDL_WINDOWEVENT_SIZE_CHANGED:
                renderer.present();
                break;
            default:
                break;
            }
        }

        renderer.set_draw_color(sdl::pallete::white);
        renderer.clear();

        constexpr int board_size = 8;
        const auto window_size = window.size();
        const auto min_dim = std::min(window_size.x, window_size.y);
        const int square_size = min_dim / board_size;
        auto rectangle = sdl::Rectangle<int>{0, 0, square_size, square_size};
        for (int row = 0; row < board_size; ++row) {
            for (int col = 0; col < board_size; ++col) {
                renderer.set_draw_color(((row + col) % 2 == 0) ? sdl::pallete::white : sdl::pallete::blue);
                rectangle.x = row * square_size;
                rectangle.y = col * square_size;
                renderer.fill_rectangle(rectangle);
            }
        }

        const auto pieces_sprites_rect = piece_sprites_grid.get_region({3, 1});
        constexpr auto screen_rect = sdl::Rectangle<int>{0, 0, 200, 200};
        renderer.copy<int>(piece_sprites, pieces_sprites_rect, screen_rect);

        renderer.present();

        minimum_frame_delay.end_interval();

        constexpr bool log_frame_duration = false;
        if (log_frame_duration) {
            const auto frame_duration = minimum_frame_delay.previous_interval_duration();
            std::cout << to_milliseconds(frame_duration) << " ms\n";
        }
    }

    return 0;
}
