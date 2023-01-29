#include "sdlpp.h"
#include "sdlpp_image.h"

#include <gsl/gsl>

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
    return {
        .x = coordinate.x * pitch_.x, .y = coordinate.y * pitch_.y, .w = pitch_.x, .h = pitch_.y};
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

int main(int argc, char *argv[])
{
    constexpr int width = 640;
    constexpr int height = 480;
    constexpr sdl::Rectangle<int> screen_region{0, 0, width, height};

    sdl::Application application{SDL_INIT_VIDEO};
    sdl::image::Extensions extensions{IMG_INIT_PNG};

    auto window = sdl::make_window("SDL Application", SDL_WINDOWPOS_UNDEFINED,
                                   SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);

    auto renderer = sdl::make_renderer(window.get(), -1, SDL_RENDERER_ACCELERATED);

    const auto color = sdl::pallete::white;
    SDL_SetRenderDrawColor(renderer.get(), color.r, color.g, color.b, color.a);

    auto piece_sprites = sdl::make_texture_from_surface(
        renderer.get(), sdl::image::load_image("resources/pieces_sprite_map.png").get());

    sdl::Point<int> piece_sprites_size;
    SDL_QueryTexture(piece_sprites.get(), nullptr, nullptr, &piece_sprites_size.x,
                     &piece_sprites_size.y);

    SpriteMapGrid piece_sprites_grid{piece_sprites_size, {n_pieces, n_colors}};

    while (application.running()) {
        application.process_events();

        int piece_width = piece_sprites_size.x / static_cast<int>(n_pieces);
        int piece_height = piece_sprites_size.y / static_cast<int>(n_colors);

        int piece_i = 3;
        int color_i = 1;
        auto pieces_sprites_rect = piece_sprites_grid.get_region({piece_i, color_i});
        const sdl::Rectangle<int> screen_rect{.x = 0, .y = 0, .w = 200, .h = 200};

        SDL_RenderClear(renderer.get());
        SDL_RenderCopy(renderer.get(), piece_sprites.get(), &pieces_sprites_rect, &screen_rect);
        SDL_RenderPresent(renderer.get());
    }

    return 0;
}
