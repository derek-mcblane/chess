#pragma once

#include "sdlpp.h"

#include "widget.h"

#include <atomic>
#include <functional>
#include <optional>

class Button : public Widget
{
  public:
    using OnClickedCallback = std::function<void()>;
    using DrawStrategy = std::function<void(sdl::Renderer&)>;

    Button(Region region = Region{}) : region_(region) {}

    void set_on_clicked_callback(OnClickedCallback&& callback);
    void clear_on_clicked_callback();

    void set_draw_strategy(DrawStrategy&& draw_strategy);
    void clear_draw_strategy();

  private:
    void on_button_down_impl(const SDL_MouseButtonEvent& event) override;
    void on_button_up_impl(const SDL_MouseButtonEvent& event) override;
    Region& region_impl() override;
    [[nodiscard]] constexpr const Region& region_impl() const override;
    void draw_impl(sdl::Renderer& renderer) const override;

    bool is_mouse_button_event_in_region(const SDL_MouseButtonEvent& event);
    void call_on_clicked_callback_if_set();

    Region region_;
    std::atomic_bool mouse_down_;
    std::optional<OnClickedCallback> callback_;
    std::optional<DrawStrategy> draw_strategy_;
};
