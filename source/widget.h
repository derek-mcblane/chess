#pragma once

#include "sdlpp.h"

class Widget
{
  public:
    using Region = sdl::Rectangle<int>;
    using Point = sdl::Point<int>;

    void on_button_down(const SDL_MouseButtonEvent& event)
    {
        on_button_down_impl(event);
    }
    void on_button_up(const SDL_MouseButtonEvent& event)
    {
        on_button_up_impl(event);
    }

    Region& region()
    {
        return region_impl();
    }

    [[nodiscard]] constexpr const Region& region() const
    {
        return region_impl();
    }

    void draw(sdl::Renderer& renderer) const
    {
        draw_impl(renderer);
    }

    void set_origin(Point origin)
    {
        region().x = origin.x;
        region().y = origin.y;
    }

    void set_size(Point size)
    {
        region().w = size.x;
        region().h = size.y;
    }

    [[nodiscard]] constexpr Point origin() const
    {
        return sdl::Point<int>{region().x, region().y};
    }

    [[nodiscard]] constexpr Point size() const
    {
        return sdl::Point<int>{region().w, region().h};
    }

  private:
    virtual void on_button_down_impl(const SDL_MouseButtonEvent& event) = 0;
    virtual void on_button_up_impl(const SDL_MouseButtonEvent& event) = 0;

    virtual Region& region_impl() = 0;
    [[nodiscard]] virtual constexpr const Region& region_impl() const = 0;

    virtual void draw_impl(sdl::Renderer& renderer) const = 0;
};
