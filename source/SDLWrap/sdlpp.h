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

using Event = SDL_Event;
using EventType = SDL_EventType;
using EventFilterCallback = int (*)(void* userdata, Event* event);

void add_event_watch(EventFilterCallback callback, void* user_data) noexcept;
void set_event_filter(EventFilterCallback callback, void* user_data) noexcept;
bool get_event_filter(EventFilterCallback* callback, void** user_data) noexcept;
bool get_event_filter(EventFilterCallback& callback, void*& user_data) noexcept;
void pump_events() noexcept;
void flush_events(EventType min_event, EventType max_event) noexcept;
void flush_all_events() noexcept;
bool poll_event(Event* event) noexcept;
bool poll_event(Event& event) noexcept;
std::optional<Event> poll_event() noexcept;
void wait_event(Event* event);
void wait_event(Event& event);
Event wait_event();
void wait_event(Event* event, int timeout);
void wait_event(Event& event, int timeout);
Event wait_event(int timeout);

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

template <typename PointT>
bool operator==(const PointT lhs, const PointT rhs)
{
    return {lhs.x == rhs.x, lhs.y == rhs.y};
}

template <typename PointT>
bool operator!=(const PointT lhs, const PointT rhs)
{
    return !(lhs == rhs);
}

template <typename PointT>
PointT operator+(const PointT lhs, const PointT rhs)
{
    return {lhs.x + rhs.x, lhs.y + rhs.y};
}

template <typename PointT>
PointT& operator+=(PointT& lhs, const PointT rhs)
{
    return lhs = lhs + rhs;
}

template <typename PointT>
PointT operator-(const PointT lhs, const PointT rhs)
{
    return {lhs.x + rhs.x, lhs.y + rhs.y};
}

template <typename PointT>
PointT& operator-=(PointT& lhs, const PointT rhs)
{
    return lhs = lhs - rhs;
}

template <typename PointT, typename U>
PointT operator*(const U lhs, const PointT rhs)
{
    return {lhs * rhs.x, lhs * rhs.y};
}

template <typename PointT, typename U>
PointT operator*(const PointT lhs, const U rhs)
{
    return {lhs.x * rhs, lhs.y * rhs};
}

template <typename PointT, typename U>
PointT& operator*=(PointT& lhs, const U rhs)
{
    return lhs = lhs * rhs;
}

template <typename PointT, typename U>
PointT operator/(const PointT lhs, const U rhs)
{
    return {lhs.x / rhs, lhs.y / rhs};
}

template <typename PointT, typename U>
PointT& operator/=(PointT& lhs, const U rhs)
{
    return lhs = lhs / rhs;
}

template <typename T>
struct select_rectangle;

template <>
struct select_rectangle<int>
{
    using type = SDL_Rect;
};

template <>
struct select_rectangle<float>
{
    using type = SDL_FRect;
};

template <typename T>
using Rectangle = typename select_rectangle<T>::type;

template <typename Rectangle>
struct rectangle_dimension;

template <typename Rectangle>
using rectangle_dimension_type = typename rectangle_dimension<Rectangle>::type;

template <>
struct rectangle_dimension<SDL_Rect>
{
    using type = int;
};

template <>
struct rectangle_dimension<SDL_FRect>
{
    using type = float;
};

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

enum class InitFlags : Uint32
{
    Timer = 0x00000001U,
    Audio = 0x00000010U,
    Video = 0x00000020U,
    Joystick = 0x00000200U,
    Haptic = 0x00001000U,
    GameController = 0x00002000U,
    Events = 0x00004000U,
    Sensor = 0x00008000U,
    NoParachute = 0x00100000U,
    Everything = Timer | Audio | Video | Joystick | Haptic | GameController | Events | Sensor | NoParachute,
};

inline void initialize(Uint32 flags)
{
    if (SDL_Init(flags) < 0) {
        throw sdl::exception::init{};
    }
}

inline void initialize(InitFlags flags)
{
    initialize(static_cast<Uint32>(flags));
}

inline void quit()
{
    SDL_Quit();
}

class Context
{
  public:
    [[nodiscard]] Context(Uint32 flags)
    {
        initialize(flags);
    }

    [[nodiscard]] Context(InitFlags flags)
    {
        initialize(flags);
    }

    [[nodiscard]] Context(const Context& other) = delete;
    Context operator=(const Context& other) = delete;

    [[nodiscard]] Context(Context&& other) noexcept = delete;
    Context operator=(Context&& other) noexcept = delete;

    ~Context() noexcept
    {
        quit();
    }
};

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

[[nodiscard]] WindowUniquePtr
make_window(const char* title, int x_position, int y_position, int width, int height, Uint32 flags);
[[nodiscard]] WindowUniquePtr make_window(const WindowConfig& config);

[[nodiscard]] RendererUniquePtr make_renderer(SDL_Window* window, int index, Uint32 flags);
[[nodiscard]] RendererUniquePtr make_renderer(SDL_Window* window, const RendererConfig& config);

[[nodiscard]] TextureUniquePtr make_texture_from_surface(SDL_Renderer* renderer, SDL_Surface* surface);

[[nodiscard]] SurfaceUniquePtr load_bmp(const std::string& filename);

[[nodiscard]] SurfaceUniquePtr
convert_surface(SurfaceUniquePtr surface, const SDL_PixelFormat* format, Uint32 flags = 0);

class Texture
{
  public:
    Texture(TextureUniquePtr texture) noexcept : texture_{std::move(texture)} {}

    [[nodiscard]] TextureUniquePtr::pointer get() const noexcept
    {
        return texture_.get();
    }

    [[nodiscard]] Uint32 format() const noexcept
    {
        Uint32 format;
        SDL_QueryTexture(get(), &format, nullptr, nullptr, nullptr);
        return format;
    }

    [[nodiscard]] int access() const noexcept
    {
        int access;
        SDL_QueryTexture(get(), nullptr, &access, nullptr, nullptr);
        return access;
    }

    [[nodiscard]] int width() const noexcept
    {
        int width;
        SDL_QueryTexture(get(), nullptr, nullptr, &width, nullptr);
        return width;
    }

    [[nodiscard]] int height() const noexcept
    {
        int height;
        SDL_QueryTexture(get(), nullptr, nullptr, nullptr, &height);
        return height;
    }

    [[nodiscard]] Point<int> size() const noexcept
    {
        Point<int> size;
        SDL_QueryTexture(get(), nullptr, nullptr, &size.x, &size.y);
        return size;
    }

    operator SDL_Texture&() const noexcept
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

    void set_draw_blend_mode(SDL_BlendMode mode) const
    {
        if (SDL_SetRenderDrawBlendMode(get(), mode) != 0) {
            throw exception::generic_error{};
        }
    }

    [[nodiscard]] SDL_BlendMode get_draw_blend_mode() const
    {
        SDL_BlendMode mode;
        if (SDL_GetRenderDrawBlendMode(get(), &mode) != 0) {
            throw exception::generic_error{};
        }
        return mode;
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

    void present() const noexcept
    {
        SDL_RenderPresent(get());
    }

    template <typename T>
    void draw_point(T point_x, T point_y) const;
    template <typename Point>
    void draw_point(Point point) const;

    void draw_line(int x_begin, int y_begin, int x_end, int y_end) const;
    void draw_line(Point<int> begin, Point<int> end) const;

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

    [[nodiscard]] std::tuple<int, int> size() const noexcept
    {
        std::tuple<int, int> size;
        SDL_GetWindowSize(get_pointer(), &std::get<0>(size), &std::get<1>(size));
        return size;
    }

    [[nodiscard]] bool shown() const noexcept
    {
        return (SDL_GetWindowFlags(get_pointer()) & SDL_WINDOW_SHOWN) != 0U;
    }

    [[nodiscard]] bool hidden() const noexcept
    {
        return (SDL_GetWindowFlags(get_pointer()) & SDL_WINDOW_HIDDEN) != 0U;
    }

  private:
    WindowUniquePtr window_;
};

} // namespace sdl
