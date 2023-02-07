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
    sdl::Application application{SDL_INIT_VIDEO};
    sdl::image::Extensions extensions{IMG_INIT_PNG};

    constexpr sdl::Rectangle<int> screen_region{0, 0, 640, 480};
    constexpr int max_frame_rate_per_second = 100;
    constexpr auto min_frame_period_seconds =
        std::chrono::milliseconds(1'000) / max_frame_rate_per_second;

    constexpr auto window_config =
        sdl::WindowConfig{.title = "SDL Application",
                          .x_position = SDL_WINDOWPOS_UNDEFINED,
                          .y_position = SDL_WINDOWPOS_UNDEFINED,
                          .width = screen_region.w,
                          .height = screen_region.h,
                          .flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE};

    constexpr auto renderer_config = sdl::RendererConfig{
        .index = -1, .flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC};

    auto window = sdl::Window{window_config};
    auto renderer = sdl::Renderer{window.get_pointer(), renderer_config};

    auto piece_sprites = renderer.make_texture_from_surface(
        sdl::image::load_image("resources/pieces_sprite_map.png").get());
    sdl::Point<int> piece_sprites_size;
    SDL_QueryTexture(piece_sprites.get(), nullptr, nullptr, &piece_sprites_size.x,
                     &piece_sprites_size.y);
    SpriteMapGrid piece_sprites_grid{piece_sprites_size, {n_pieces, n_colors}};

    bool running{true};
    auto previous_frame_start_time = std::chrono::steady_clock::now();
    while (running) {
        std::cout << "loop\n";
        auto frame_start_time = std::chrono::steady_clock::now();
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

        int piece_width = piece_sprites_size.x / static_cast<int>(n_pieces);
        int piece_height = piece_sprites_size.y / static_cast<int>(n_colors);

        int piece_i = 3;
        int color_i = 1;
        auto pieces_sprites_rect = piece_sprites_grid.get_region({piece_i, color_i});
        const auto screen_rect = sdl::Rectangle<int>{0, 0, 200, 200};

        renderer.set_draw_color(sdl::pallete::white);
        renderer.clear();

        renderer.copy<int>(*piece_sprites, pieces_sprites_rect, screen_rect);

        renderer.set_draw_color(sdl::pallete::gray);
        renderer.fill_rectangle<int>(sdl::Rectangle<int>{250, 250, 50, 50});

        renderer.present();

        std::this_thread::sleep_until(previous_frame_start_time + min_frame_period_seconds);
        previous_frame_start_time = frame_start_time;
    }

    return 0;
}
