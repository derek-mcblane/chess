#pragma once

#include "sdlpp.h"
#include <atomic>

class Clickable
{
  public:
    using OnClickedCallback = std::function<void()>;

    void on_mouse_down(const SDL_MouseButtonEvent& event);
    void on_mouse_up(const SDL_MouseButtonEvent& event);

    void set_on_clicked_callback(OnClickedCallback&& callback);
    void clear_on_clicked_callback();

  private:
    [[nodiscard]] virtual bool is_mouse_button_event_in_region(const SDL_MouseButtonEvent& event) = 0;
    virtual void set_on_clicked_callback_impl(OnClickedCallback&& callback) = 0;
    virtual void clear_on_clicked_callback_impl() = 0;
    virtual void call_on_clicked_callback_if_set() = 0;
    std::atomic_bool mouse_down_;
};

class Button : public Clickable
{
    using Rectangle = sdl::Rectangle<int>;

  public:
    Button(Rectangle region = Rectangle{}, std::optional<OnClickedCallback>&& clicked_callback = std::nullopt)
        : region(region), callback_{std::move(clicked_callback)}
    {}

    Rectangle region;

  private:
    void set_on_clicked_callback_impl(OnClickedCallback&& callback) override
    {
        callback_ = std::move(callback);
    }

    void clear_on_clicked_callback_impl() override
    {
        callback_ = std::nullopt;
    }

    void call_on_clicked_callback_if_set() override
    {
        if (callback_.has_value()) {
            (*callback_)();
        }
    }

    bool is_mouse_button_event_in_region(const SDL_MouseButtonEvent& event) override
    {
        const auto position = sdl::Point<int>{event.x, event.y};
        return sdl::is_point_in_rectangle(position, region);
    }

    std::optional<OnClickedCallback> callback_;
};
