#pragma once

#include "sdlpp.h"

#include <atomic>
#include <functional>

class GridView
{
  public:
    using Point = sdl::Point<int>;
    using OnClickedCallback = std::function<void(const Point&)>;

    GridView() : GridView(Point{}, Point{}) {}

    GridView(Point grid_size, Point pixel_size) : grid_size(grid_size), pixel_size(pixel_size) {}

    Point origin{0, 0};
    Point grid_size;
    Point pixel_size;

    [[nodiscard]] sdl::Rectangle<int> grid_cell(Point index) const;
    [[nodiscard]] Point grid_index(Point position) const;
    [[nodiscard]] Point grid_cell_position(Point index) const;
    [[nodiscard]] Point cell_size() const;

    void set_on_cell_clicked_callback(OnClickedCallback&& callback);
    void clear_on_cell_clicked_callback();

    void on_button_down(const SDL_MouseButtonEvent& event);
    void on_button_up(const SDL_MouseButtonEvent& event);

  private:
    std::atomic<Point> down_index_;
    std::optional<std::atomic<Point>> selected_index_;
    std::optional<OnClickedCallback> on_cell_clicked_;

    [[nodiscard]] sdl::Rectangle<int> grid_cell_local(Point index) const;
    [[nodiscard]] Point grid_index_local(Point position) const;
    [[nodiscard]] Point grid_cell_position_local(Point index) const;
};
