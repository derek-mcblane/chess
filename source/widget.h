#pragma once

#include "sdlpp.h"

class Widget
{
  public:
    void on_button_down(const SDL_MouseButtonEvent& event)
    {
        on_button_down_impl(event);
    }
    void on_button_up(const SDL_MouseButtonEvent& event)
    {
        on_button_up_impl(event);
    }

    sdl::Rectangle<int>& region()
    {
        return region_impl();
    }

    [[nodiscard]] constexpr const sdl::Rectangle<int>& region() const
    {
        return region_impl();
    }

    void draw(sdl::Renderer& renderer) const
    {
        draw_impl(renderer);
    }

    [[nodiscard]] constexpr sdl::Point<int> origin() const
    {
        return sdl::Point<int>{region().x, region().y};
    }

    void set_origin(const sdl::Point<int> origin)
    {
        region().x = origin.x;
        region().y = origin.y;
    }

    [[nodiscard]] constexpr sdl::Point<int> size() const
    {
        return sdl::Point<int>{region().w, region().h};
    }

    [[nodiscard]] constexpr sdl::Point<float> size_f() const
    {
        return sdl::Point<float>{static_cast<float>(region().w), static_cast<float>(region().h)};
    }

    void set_size(const sdl::Point<int> size)
    {
        region().w = size.x;
        region().h = size.y;
    }

    [[nodiscard]] constexpr int area() const
    {
        return region().w * region().h;
    }

    [[nodiscard]] constexpr float area_f() const
    {
        return static_cast<float>(region().w * region().h);
    }

  private:
    virtual void on_button_down_impl(const SDL_MouseButtonEvent& event) = 0;
    virtual void on_button_up_impl(const SDL_MouseButtonEvent& event) = 0;

    virtual sdl::Rectangle<int>& region_impl() = 0;
    [[nodiscard]] virtual constexpr const sdl::Rectangle<int>& region_impl() const = 0;

    virtual void draw_impl(sdl::Renderer& renderer) const = 0;
};
