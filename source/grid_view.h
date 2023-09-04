#pragma once

#include "sdlpp.h"
#include "widget.h"

#include <atomic>
#include <functional>

class ClickableGrid : public Widget
{
  public:
    using OnClickedCallback = std::function<void(const Point&)>;

    ClickableGrid(Point grid_size, Region region) : grid_size(grid_size), region_(region) {}

    Point grid_size;

    [[nodiscard]] sdl::Rectangle<int> grid_cell_local(Point index) const;
    [[nodiscard]] sdl::Rectangle<int> grid_cell(Point index) const;
    [[nodiscard]] Point grid_index_local(Point position) const;
    [[nodiscard]] Point grid_index(Point position) const;
    [[nodiscard]] Point grid_cell_position_local(Point index) const;
    [[nodiscard]] Point grid_cell_position(Point index) const;
    [[nodiscard]] Point cell_size() const;

    std::optional<Point> clicked_index() const
    {
        return clicked_index_;
    }

    void set_on_cell_clicked_callback(OnClickedCallback&& callback);
    void clear_on_cell_clicked_callback();

    [[nodiscard]] const sdl::Texture& texture() const
    {
        return texture_;
    }

    [[nodiscard]] sdl::Texture& texture()
    {
        return texture_;
    }

  private:
    Region region_;
    sdl::Texture texture_;
    std::atomic<Point> down_index_;
    std::optional<std::atomic<Point>> clicked_index_;
    std::optional<OnClickedCallback> on_cell_clicked_;

    void on_button_down_impl(const SDL_MouseButtonEvent& event) override;
    void on_button_up_impl(const SDL_MouseButtonEvent& event) override;
    Region& region_impl() override;
    [[nodiscard]] const Region& region_impl() const override;
    void draw_impl(sdl::Renderer& renderer) const override;
};
