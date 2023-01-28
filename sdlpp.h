#pragma once

#include <SDL.h>

#include <gsl/gsl>

#include <chrono>
#include <functional>
#include <memory>
#include <optional>
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
class Point : public select_point<T>::type
{};

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
class Rectangle : public select_rect<T>::type
{
    using RectangleType = typename select_rect<T>::type;

  public:
    Rectangle(Point<T> position, Point<T> size)
        : SDL_Rect{.x = position.x, .y = position.y, .w = size.x, .h = size.y}
    {}

    [[nodiscard]] const RectangleType *get() const
    {
        return static_cast<const RectangleType *>(this);
    }
};

namespace exception {

class generic_error : virtual public std::runtime_error
{
  public:
    [[nodiscard]] generic_error() : std::runtime_error(SDL_GetError()) {}

    [[nodiscard]] generic_error(const generic_error &other) noexcept = default;
    generic_error &operator=(const generic_error &other) noexcept    = default;

    [[nodiscard]] generic_error(generic_error &&other) noexcept = default;
    generic_error &operator=(generic_error &&other) noexcept    = default;

    ~generic_error() noexcept override = default;

    [[nodiscard]] const char *error() const noexcept
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

class Application
{
  public:
    [[nodiscard]] Application(std::uint32_t flags) : flags_{flags}
    {
        namespace chrono = std::chrono;

        if (SDL_Init(flags_) < 0) {
            throw exception::init{};
        }
        running_                 = true;
        last_event_process_time_ = chrono::steady_clock::now();
    }

    [[nodiscard]] Application(const Application &other) = delete;
    Application operator=(const Application &other)     = delete;

    [[nodiscard]] Application(Application &&other) noexcept = delete;
    Application operator=(Application &&other) noexcept     = delete;

    ~Application() noexcept
    {
        SDL_Quit();
    }

    [[nodiscard]] bool running() const noexcept
    {
        return running_;
    }

    void process_events()
    {
        namespace chrono = std::chrono;
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
                on_sdl_quit_event();
                break;
            default:
                break;
            }
        }
        auto event_process_time_ = chrono::steady_clock::now();
        std::this_thread::sleep_until(last_event_process_time_ + chrono::milliseconds(25));
        last_event_process_time_ = event_process_time_;
    }

  private:
    bool running_{false};
    Uint32 flags_;
    std::chrono::steady_clock::time_point last_event_process_time_;

    void on_sdl_quit_event() noexcept
    {
        running_ = false;
    }
};

struct WindowDeleter
{
    void operator()(SDL_Window *window) noexcept
    {
        SDL_DestroyWindow(window);
    }
};

struct RendererDeleter
{
    void operator()(SDL_Renderer *window) noexcept
    {
        SDL_DestroyRenderer(window);
    }
};

struct TextureDeleter
{
    void operator()(SDL_Texture *surface) noexcept
    {
        SDL_DestroyTexture(surface);
    }
};

struct SurfaceDeleter
{
    void operator()(SDL_Surface *surface) noexcept
    {
        SDL_FreeSurface(surface);
    }
};

using WindowUniquePtr   = std::unique_ptr<SDL_Window, WindowDeleter>;
using RendererUniquePtr = std::unique_ptr<SDL_Renderer, RendererDeleter>;
using TextureUniquePtr  = std::unique_ptr<SDL_Texture, TextureDeleter>;
using SurfaceUniquePtr  = std::unique_ptr<SDL_Surface, SurfaceDeleter>;

[[nodiscard]] WindowUniquePtr make_window(const char *title, int x, int y, int w, int h,
                                          Uint32 flags);

[[nodiscard]] RendererUniquePtr make_renderer(SDL_Window *window, int index, Uint32 flags);

[[nodiscard]] TextureUniquePtr make_texture_from_surface(SDL_Renderer *renderer,
                                                         SDL_Surface *surface);

[[nodiscard]] SurfaceUniquePtr load_bmp(const std::string &filename);

[[nodiscard]] SurfaceUniquePtr convert_surface(SurfaceUniquePtr surface,
                                               const SDL_PixelFormat *format, Uint32 flags = 0);

} // namespace sdl
