#include "grid_view.h"

#include "sdlpp.h"

#include <spdlog/spdlog.h>

using Point = GridView::Point;

[[nodiscard]] sdl::Rectangle<int> GridView::grid_cell(Point index) const
{
    using namespace sdl;
    return grid_cell_local(index) + origin;
}

[[nodiscard]] sdl::Rectangle<int> GridView::grid_cell_local(Point index) const
{
    const auto width = cell_size().x;
    const auto height = cell_size().y;
    return {index.x * width, index.y * height, width, height};
}

[[nodiscard]] Point GridView::grid_index(Point position) const
{
    using namespace sdl;
    return grid_index_local(position - origin);
}

[[nodiscard]] Point GridView::grid_index_local(const Point position) const
{
    return {position.x / cell_size().x, position.y / cell_size().y};
}

[[nodiscard]] Point GridView::grid_cell_position(Point index) const
{
    using namespace sdl;
    return grid_cell_position_local(index) + origin;
}

[[nodiscard]] Point GridView::grid_cell_position_local(Point index) const
{
    return {index.x * cell_size().x, index.y * cell_size().y};
}

[[nodiscard]] Point GridView::cell_size() const
{
    return {pixel_size.x / grid_size.x, pixel_size.y / grid_size.y};
}

void GridView::set_on_cell_clicked_callback(OnClickedCallback&& callback)
{
    on_cell_clicked_ = std::move(callback);
}

void GridView::clear_on_cell_clicked_callback()
{
    on_cell_clicked_.reset();
}

void GridView::on_button_down(const SDL_MouseButtonEvent& event)
{
    using namespace sdl;
    selected_index_.reset();
    const auto click_position = Point{event.x, event.y};
    Point clicked_index{grid_index(click_position)};
    if (clicked_index.x < 0 || clicked_index.x >= grid_size.x) {
        return;
    }
    if (clicked_index.y < 0 || clicked_index.y >= grid_size.y) {
        return;
    }
    down_index_ = clicked_index;
}

void GridView::on_button_up(const SDL_MouseButtonEvent& event)
{
    using namespace sdl;
    const Point up_index = grid_index({event.x, event.y});
    if (up_index == down_index_.load()) {
        selected_index_ = up_index;
        if (on_cell_clicked_) {
            (*on_cell_clicked_)(up_index);
        }
    }
}
