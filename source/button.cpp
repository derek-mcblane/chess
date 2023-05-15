#include "button.h"

void Button::set_on_clicked_callback(OnClickedCallback&& callback)
{
    callback_ = std::move(callback);
}
void Button::clear_on_clicked_callback()
{
    callback_ = std::nullopt;
}

void Button::set_draw_strategy(DrawStrategy&& draw_strategy)
{
    draw_strategy_ = std::move(draw_strategy);
}
void Button::clear_draw_strategy()
{
    draw_strategy_ = std::nullopt;
}

void Button::on_button_down_impl(const SDL_MouseButtonEvent& event)
{
    if (is_mouse_button_event_in_region(event)) {
        mouse_down_ = true;
    }
}

void Button::on_button_up_impl(const SDL_MouseButtonEvent& event)
{
    if (is_mouse_button_event_in_region(event) && mouse_down_) {
        call_on_clicked_callback_if_set();
    }
}

Button::Region& Button::region_impl()
{
    return region_;
}

constexpr const Button::Region& Button::region_impl() const
{
    return region_;
}

void Button::draw_impl(sdl::Renderer& renderer) const
{
    if (draw_strategy_.has_value()) {
        (*draw_strategy_)(renderer);
    }
}

bool Button::is_mouse_button_event_in_region(const SDL_MouseButtonEvent& event)
{
    const auto position = sdl::Point<int>{event.x, event.y};
    return sdl::is_point_in_rectangle(position, region_);
}

void Button::call_on_clicked_callback_if_set()
{
    if (callback_.has_value()) {
        (*callback_)();
    }
}
