#include "sdlpp.h"

namespace sdl {

[[nodiscard]] WindowUniquePtr make_window(const char *title, int x_position, int y_position,
                                          int width, int height, Uint32 flags)
{
    WindowUniquePtr window{SDL_CreateWindow(title, x_position, y_position, width, height, flags)};
    if (window == nullptr) {
        throw sdl::exception::create_window{};
    }
    return window;
}

[[nodiscard]] WindowUniquePtr make_window(const WindowConfig &config)
{
    return make_window(config.title, config.x_position, config.y_position, config.width,
                       config.height, config.flags);
}

[[nodiscard]] RendererUniquePtr make_renderer(SDL_Window *window, int index, Uint32 flags)
{
    RendererUniquePtr renderer{SDL_CreateRenderer(window, index, flags)};
    if (renderer == nullptr) {
        throw sdl::exception::create_renderer{};
    }
    return renderer;
}

[[nodiscard]] RendererUniquePtr make_renderer(SDL_Window *window, const RendererConfig &config)
{
    return make_renderer(window, config.index, config.flags);
}

[[nodiscard]] TextureUniquePtr make_texture_from_surface(SDL_Renderer *renderer,
                                                         SDL_Surface *surface)
{
    TextureUniquePtr texture{SDL_CreateTextureFromSurface(renderer, surface)};
    if (texture == nullptr) {
        throw sdl::exception::texture_from_surface{};
    }
    return texture;
}

[[nodiscard]] TextureUniquePtr Renderer::make_texture_from_surface(SDL_Surface *surface)
{
    return ::sdl::make_texture_from_surface(renderer_.get(), surface);
}

[[nodiscard]] SurfaceUniquePtr load_bmp(const std::string &filename)
{
    SurfaceUniquePtr image{SDL_LoadBMP(filename.c_str())};
    if (image == nullptr) {
        throw sdl::exception::load_image{};
    }
    return image;
}

[[nodiscard]] SurfaceUniquePtr convert_surface(SurfaceUniquePtr surface,
                                               const SDL_PixelFormat *format, Uint32 flags)
{
    SurfaceUniquePtr converted_surface{SDL_ConvertSurface(surface.get(), format, flags)};
    if (converted_surface == nullptr) {
        throw sdl::exception::convert_surface{};
    }
    return converted_surface;
}

template <>
void Renderer::fill_rectangle<int>(const Rectangle<int> &rectangle)
{
    if (SDL_RenderFillRect(renderer_.get(), &rectangle) != 0) {
        throw exception::generic_error{};
    }
}

template <>
void Renderer::fill_rectangle<float>(const Rectangle<float> &rectangle)
{
    if (SDL_RenderFillRectF(renderer_.get(), &rectangle) != 0) {
        throw exception::generic_error{};
    }
}

template <>
void Renderer::fill_rectangles<int>(std::span<Rectangle<int>> rectangles)
{
    if (SDL_RenderFillRects(renderer_.get(), rectangles.data(),
                            gsl::narrow<int>(rectangles.size())) != 0) {
        throw exception::generic_error{};
    }
}

template <>
void Renderer::fill_rectangles<float>(std::span<Rectangle<float>> rectangles)
{
    if (SDL_RenderFillRectsF(renderer_.get(), rectangles.data(),
                             gsl::narrow<int>(rectangles.size())) != 0) {
        throw exception::generic_error{};
    }
}

template <>
void Renderer::copy<int>(SDL_Texture &texture, const Rectangle<int> &source,
                         const Rectangle<int> &destination)
{
    if (SDL_RenderCopy(renderer_.get(), &texture, &source, &destination) != 0) {
        throw exception::generic_error{};
    }
}

template <>
void Renderer::copy<float>(SDL_Texture &texture, const Rectangle<int> &source,
                           const Rectangle<float> &destination)
{
    if (SDL_RenderCopyF(renderer_.get(), &texture, &source, &destination) != 0) {
        throw exception::generic_error{};
    }
}

} // namespace sdl
