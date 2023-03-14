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

    Point pixel_size;
    Point grid_size;

    [[nodiscard]] sdl::Rectangle<float> grid_element(Point index) const;
    [[nodiscard]] Point grid_index(Point position) const;
    [[nodiscard]] Point grid_element_position(Point index) const;
    [[nodiscard]] Point element_size() const;

    void set_on_cell_clicked_callback(OnClickedCallback&& callback);
    void clear_on_cell_clicked_callback();

    void on_button_down(const SDL_MouseButtonEvent& event);
    void on_button_up(const SDL_MouseButtonEvent& event);

  private:
    std::atomic<Point> down_index_;
    std::optional<std::atomic<Point>> selected_index_;
    std::optional<OnClickedCallback> on_cell_clicked_;
};
