#pragma once

#include "sdlpp.h"
#include "widget.h"

#include <atomic>
#include <functional>

class ClickableGrid : public Widget
{
  public:
    using OnClickedCallback = std::function<void(const Point&)>;

    ClickableGrid(Point grid_size = Point{}, Region region = Region{}) : grid_size(grid_size), region(region) {}

    Point grid_size;
    Region region;

    [[nodiscard]] sdl::Rectangle<int> grid_cell(Point index) const;
    [[nodiscard]] Point grid_index(Point position) const;
    [[nodiscard]] Point grid_cell_position(Point index) const;
    [[nodiscard]] Point cell_size() const;

    [[nodiscard]] Point origin() const
    {
        return sdl::Point<int>{region.x, region.y};
    }

    [[nodiscard]] Point size() const
    {
        return sdl::Point<int>{region.w, region.h};
    }

    void set_on_cell_clicked_callback(OnClickedCallback&& callback);
    void clear_on_cell_clicked_callback();

  private:
    std::atomic<Point> down_index_;
    std::optional<std::atomic<Point>> selected_index_;
    std::optional<OnClickedCallback> on_cell_clicked_;

    [[nodiscard]] sdl::Rectangle<int> grid_cell_local(Point index) const;
    [[nodiscard]] Point grid_index_local(Point position) const;
    [[nodiscard]] Point grid_cell_position_local(Point index) const;

    void on_button_down_impl(const SDL_MouseButtonEvent& event) override;
    void on_button_up_impl(const SDL_MouseButtonEvent& event) override;
};
