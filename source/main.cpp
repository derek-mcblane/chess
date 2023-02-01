#include "sdlpp.h"
#include "sdlpp_image.h"

#include <SDL_render.h>
#include <gsl/gsl>

#include <exception>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

namespace sdl::pallete {

constexpr Color black{0x00, 0x00, 0x00, 0xFF};
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
    auto region = sdl::Rectangle<int>{
        .x = coordinate.x * pitch_.x, .y = coordinate.y * pitch_.y, .w = pitch_.x, .h = pitch_.y};
    return region;
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

    auto renderer = sdl::Renderer{sdl::make_renderer(window.get(), -1, SDL_RENDERER_ACCELERATED)};

    renderer.set_draw_color(sdl::pallete::white);

    auto piece_sprites = renderer.make_texture_from_surface(
        sdl::image::load_image("resources/pieces_sprite_map.png").get());

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
        const auto screen_rect = sdl::Rectangle<int>{.x = 0, .y = 0, .w = 200, .h = 200};

        renderer.clear();
        renderer.copy<int>(*piece_sprites, pieces_sprites_rect, screen_rect);
        renderer.present();
    }

    return 0;
}
