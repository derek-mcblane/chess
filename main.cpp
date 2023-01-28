#include "sdlpp.h"
#include "sdlpp_image.h"

#include <exception>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

namespace sdl {
struct Color
{
    Uint8 r;
    Uint8 g;
    Uint8 b;
    Uint8 a;
};

namespace pallete {

constexpr Color black{0x00, 0x00, 0x00, 0xFF};
constexpr Color white{0xFF, 0xFF, 0xFF, 0xFF};

} // namespace pallete
} // namespace sdl

constexpr int white    = 0;
constexpr int black    = 1;
constexpr int n_colors = 2;

constexpr int king     = 0;
constexpr int queen    = 1;
constexpr int bishop   = 2;
constexpr int knight   = 3;
constexpr int rook     = 4;
constexpr int pawn     = 5;
constexpr int n_pieces = 6;

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
    return {{coordinate.x * pitch_.x, coordinate.y * pitch_.y}, {pitch_.x, pitch_.y}};
}

int main(int argc, char *argv[])
{
    constexpr int width  = 640;
    constexpr int height = 480;

    sdl::Application application{SDL_INIT_VIDEO};
    sdl::image::Extensions extensions{IMG_INIT_PNG};

    auto window   = sdl::make_window("SDL Application", SDL_WINDOWPOS_UNDEFINED,
                                     SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
    auto renderer = sdl::make_renderer(window.get(), -1, SDL_RENDERER_ACCELERATED);

    const auto color = sdl::pallete::white;
    SDL_SetRenderDrawColor(renderer.get(), color.r, color.g, color.b, color.a);

    auto pieces_sprite_map_image = sdl::image::load_image("resources/pieces_sprite_map.png");

    auto pieces_grid = SpriteMapGrid{{pieces_sprite_map_image->w, pieces_sprite_map_image->h},
                                     {n_pieces, n_colors}};
    auto pieces_sprite_map =
        sdl::make_texture_from_surface(renderer.get(), pieces_sprite_map_image.get());

    while (application.running()) {
        application.process_events();

        const sdl::Rectangle<int> screen_region{{0, 0}, {width, height}};
        auto piece_region = pieces_grid.get_region({white, knight});

        SDL_RenderClear(renderer.get());
        SDL_RenderCopy(renderer.get(), pieces_sprite_map.get(), piece_region.get(), screen_region.get());
        SDL_RenderPresent(renderer.get());
    }

    return 0;
}
