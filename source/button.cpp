#include "button.h"

void Clickable::on_mouse_down(const SDL_MouseButtonEvent& event) {
    if (is_mouse_button_event_in_region(event)) {
        mouse_down_ = true;
    }
}

void Clickable::on_mouse_up(const SDL_MouseButtonEvent& event) {
    if (is_mouse_button_event_in_region(event) && mouse_down_) {
    }
}

