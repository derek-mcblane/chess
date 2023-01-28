#include "sdlpp.h"

namespace sdl {

[[nodiscard]] WindowUniquePtr
make_window(const char *title, int x, int y, int w, int h, Uint32 flags)
{
    WindowUniquePtr window{SDL_CreateWindow(title, x, y, w, h, flags)};
    if (window == nullptr) {
        throw sdl::exception::create_window{};
    }
    return window;
}

[[nodiscard]] RendererUniquePtr make_renderer(SDL_Window *window,
                                                                           int index, Uint32 flags)
{
    RendererUniquePtr renderer{
        SDL_CreateRenderer(window, index, flags)};
    if (renderer == nullptr) {
        throw sdl::exception::create_renderer{};
    }
    return renderer;
}

[[nodiscard]] TextureUniquePtr
make_texture_from_surface(SDL_Renderer *renderer, SDL_Surface *surface)
{
    TextureUniquePtr texture{
        SDL_CreateTextureFromSurface(renderer, surface)};
    if (texture == nullptr) {
        throw sdl::exception::texture_from_surface{};
    }
    return texture;
}

[[nodiscard]] SurfaceUniquePtr load_bmp(const std::string &filename)
{
    SurfaceUniquePtr image{SDL_LoadBMP(filename.c_str())};
    if (image == nullptr) {
        throw sdl::exception::load_image{};
    }
    return image;
}

[[nodiscard]] SurfaceUniquePtr
convert_surface(SurfaceUniquePtr surface, const SDL_PixelFormat *format,
                Uint32 flags)
{
    SurfaceUniquePtr converted_surface{
        SDL_ConvertSurface(surface.get(), format, flags)};
    if (converted_surface == nullptr) {
        throw sdl::exception::convert_surface{};
    }
    return converted_surface;
}

} // namespace sdl
