#pragma once

#include "sdlpp.h"
#include <atomic>

class Clickable
{
  public:
    void on_mouse_down(const SDL_MouseButtonEvent& event);
    void on_mouse_up(const SDL_MouseButtonEvent& event);
    void emit_clicked_event();

  private:
    [[nodiscard]] virtual bool is_mouse_button_event_in_region(const SDL_MouseButtonEvent& event) = 0;
    std::atomic_bool mouse_down_;
};
