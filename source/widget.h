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

  private:
    virtual void on_button_down_impl(const SDL_MouseButtonEvent& event) = 0;
    virtual void on_button_up_impl(const SDL_MouseButtonEvent& event) = 0;
};
