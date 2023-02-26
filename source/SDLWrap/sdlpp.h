#pragma once

#include <SDL.h>

#include <gsl/gsl>

#include <chrono>
#include <functional>
#include <iostream>
#include <memory>
#include <optional>
#include <span>
#include <stdexcept>
#include <string>
#include <thread>

namespace sdl {

template <typename T>
struct select_point;

template <>
struct select_point<int>
{
    using type = SDL_Point;
};

template <>
struct select_point<float>
{
    using type = SDL_FPoint;
};

template <typename T>
using Point = typename select_point<T>::type;

template <typename T>
struct select_rect;

template <>
struct select_rect<int>
{
    using type = SDL_Rect;
};

template <>
struct select_rect<float>
{
    using type = SDL_FRect;
};

template <typename T>
using Rectangle = typename select_rect<T>::type;

using Color = SDL_Color;

struct RendererConfig
{
    int index;
    Uint32 flags;
};

struct WindowConfig
{
    const char* title;
    int x_position;
    int y_position;
    int width;
    int height;
    Uint32 flags;
};

namespace exception {

class generic_error : virtual public std::runtime_error
{
  public:
    [[nodiscard]] generic_error() : std::runtime_error(SDL_GetError()) {}

    [[nodiscard]] generic_error(const generic_error& other) noexcept = default;
    generic_error& operator=(const generic_error& other) noexcept = default;

    [[nodiscard]] generic_error(generic_error&& other) noexcept = default;
    generic_error& operator=(generic_error&& other) noexcept = default;

    ~generic_error() noexcept override = default;

    [[nodiscard]] static const char* error() noexcept
    {
        return SDL_GetError();
    }
};

class init final : virtual public generic_error
{
  public:
    [[nodiscard]] init() : std::runtime_error(SDL_GetError()) {}
};

class create_window final : virtual public generic_error
{
  public:
    [[nodiscard]] create_window() : std::runtime_error(SDL_GetError()) {}
};

class create_renderer final : virtual public generic_error
{
  public:
    [[nodiscard]] create_renderer() : std::runtime_error(SDL_GetError()) {}
};

class load_image final : virtual public generic_error
{
  public:
    [[nodiscard]] load_image() : std::runtime_error(SDL_GetError()) {}
};

class convert_surface final : virtual public generic_error
{
  public:
    [[nodiscard]] convert_surface() : std::runtime_error(SDL_GetError()) {}
};

class texture_from_surface final : virtual public generic_error
{
  public:
    [[nodiscard]] texture_from_surface() : std::runtime_error(SDL_GetError()) {}
};

} // namespace exception

struct WindowDeleter
{
    void operator()(SDL_Window* window) noexcept
    {
        SDL_DestroyWindow(window);
    }
};

struct RendererDeleter
{
    void operator()(SDL_Renderer* window) noexcept
    {
        SDL_DestroyRenderer(window);
    }
};

struct TextureDeleter
{
    void operator()(SDL_Texture* surface) noexcept
    {
        SDL_DestroyTexture(surface);
    }
};

struct SurfaceDeleter
{
    void operator()(SDL_Surface* surface) noexcept
    {
        SDL_FreeSurface(surface);
    }
};

using WindowUniquePtr = std::unique_ptr<SDL_Window, WindowDeleter>;
using RendererUniquePtr = std::unique_ptr<SDL_Renderer, RendererDeleter>;
using TextureUniquePtr = std::unique_ptr<SDL_Texture, TextureDeleter>;
using SurfaceUniquePtr = std::unique_ptr<SDL_Surface, SurfaceDeleter>;

[[nodiscard]] WindowUniquePtr make_window(const char* title, int x_position, int y_position, int width, int height,
                                          Uint32 flags);
[[nodiscard]] WindowUniquePtr make_window(const WindowConfig& config);

[[nodiscard]] RendererUniquePtr make_renderer(SDL_Window* window, int index, Uint32 flags);
[[nodiscard]] RendererUniquePtr make_renderer(SDL_Window* window, const RendererConfig& config);

[[nodiscard]] TextureUniquePtr make_texture_from_surface(SDL_Renderer* renderer, SDL_Surface* surface);

[[nodiscard]] SurfaceUniquePtr load_bmp(const std::string& filename);

[[nodiscard]] SurfaceUniquePtr convert_surface(SurfaceUniquePtr surface, const SDL_PixelFormat* format,
                                               Uint32 flags = 0);

class Texture
{
  public:
    Texture(TextureUniquePtr texture) : texture_{std::move(texture)} {}

    [[nodiscard]] TextureUniquePtr::pointer get() const noexcept
    {
        return texture_.get();
    }

    [[nodiscard]] Uint32 format() const
    {
        Uint32 format;
        SDL_QueryTexture(get(), &format, nullptr, nullptr, nullptr);
        return format;
    }

    [[nodiscard]] int access() const
    {
        int access;
        SDL_QueryTexture(get(), nullptr, &access, nullptr, nullptr);
        return access;
    }

    [[nodiscard]] int width() const
    {
        int width;
        SDL_QueryTexture(get(), nullptr, nullptr, &width, nullptr);
        return width;
    }

    [[nodiscard]] int height() const
    {
        int height;
        SDL_QueryTexture(get(), nullptr, nullptr, nullptr, &height);
        return height;
    }

    [[nodiscard]] Point<int> size() const
    {
        Point<int> size;
        SDL_QueryTexture(get(), nullptr, nullptr, &size.x, &size.y);
        return size;
    }

    operator SDL_Texture&() const
    {
        return *get();
    }

  private:
    TextureUniquePtr texture_;
};

class Renderer
{
  public:
    Renderer(SDL_Window* window, int index, Uint32 flags) : renderer_{make_renderer(window, index, flags)} {}

    Renderer(SDL_Window* window, const RendererConfig& config) : renderer_{make_renderer(window, config)} {}

    Renderer(RendererUniquePtr renderer) : renderer_{std::move(renderer)} {}

    [[nodiscard]] RendererUniquePtr::pointer get() const noexcept
    {
        return renderer_.get();
    }

    void set_draw_color(const Color& color) const
    {
        if (SDL_SetRenderDrawColor(get(), color.r, color.g, color.b, color.a) != 0) {
            throw exception::generic_error{};
        }
    }

    void clear() const
    {
        if (SDL_RenderClear(get()) != 0) {
            throw exception::generic_error{};
        }
    }

    void present() const
    {
        SDL_RenderPresent(get());
    }

    template <typename Rectangle>
    void fill_rectangle(const Rectangle& rectangle);

    template <typename Rectangle>
    void fill_rectangles(std::span<Rectangle> rectangles);

    template <typename DestinationT>
    void copy(SDL_Texture& texture, const Rectangle<int>& source, const Rectangle<DestinationT>& destination);

    TextureUniquePtr make_texture_from_surface(SDL_Surface* surface);

  private:
    RendererUniquePtr renderer_;
};

class Window
{
    template <typename T>
    using OptionalReference = std::optional<std::reference_wrapper<T>>;

  public:
    Window(const char* title, int x_position, int y_position, int width, int height, Uint32 flags)
        : Window(make_window(title, x_position, y_position, width, height, flags))
    {}

    Window(const WindowConfig& config)
        : Window(config.title, config.x_position, config.y_position, config.width, config.height, config.flags)
    {}

    Window(WindowUniquePtr window) : window_(std::move(window)) {}

    [[nodiscard]] WindowUniquePtr::pointer get_pointer() const noexcept
    {
        return window_.get();
    }

    void enable_renderer(const RendererConfig& config) {}

  private:
    WindowUniquePtr window_;
};

class Application
{
  public:
    [[nodiscard]] Application(Uint32 flags)
    {
        if (SDL_Init(flags) < 0) {
            throw exception::init{};
        }
    }

    [[nodiscard]] Application(const Application& other) = delete;
    Application operator=(const Application& other) = delete;

    [[nodiscard]] Application(Application&& other) noexcept = delete;
    Application operator=(Application&& other) noexcept = delete;

    ~Application() noexcept
    {
        SDL_Quit();
    }
};

} // namespace sdl
