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
constexpr Color white{0xFF, 0xFF, 0xFF, 0xFF};
constexpr Color gray{0x0F, 0x0F, 0x0F, 0x0F};

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

int main(int argc, char* argv[])
{
    sdl::Application application{SDL_INIT_VIDEO};
    sdl::image::Extensions extensions{IMG_INIT_PNG};

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
            case SDL_WINDOWEVENT_ENTER:
                std::cout << "mouse enter\n";
                break;
            default:
                break;
            }
        }

        renderer.set_draw_color(sdl::pallete::white);
        renderer.clear();

        const auto pieces_sprites_rect = piece_sprites_grid.get_region({3, 1});
        constexpr auto screen_rect = sdl::Rectangle<int>{0, 0, 200, 200};
        renderer.copy<int>(piece_sprites, pieces_sprites_rect, screen_rect);

        renderer.set_draw_color(sdl::pallete::gray);
        constexpr auto rectangle = sdl::Rectangle<int>{250, 250, 50, 50};
        renderer.fill_rectangle(rectangle);

        renderer.present();

        minimum_frame_delay.end_interval();
        const auto frame_duration = minimum_frame_delay.previous_interval_duration();
        std::cout << to_milliseconds(frame_duration) << " ms\n";
    }

    return 0;
}
