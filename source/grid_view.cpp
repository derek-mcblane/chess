#include "grid_view.h"

#include "sdlpp.h"

using Point = GridView::Point;

[[nodiscard]] sdl::Rectangle<float> GridView::grid_element(Point index) const
{
    const auto width = static_cast<float>(element_size().x);
    const auto height = static_cast<float>(element_size().y);
    return {static_cast<float>(index.x) * width, static_cast<float>(index.y) * height, width, height};
}

[[nodiscard]] Point GridView::grid_index(Point position) const
{
    return {position.x / element_size().x, position.y / element_size().y};
}

[[nodiscard]] Point GridView::grid_element_position(Point index) const
{
    return {index.x * element_size().x, index.y * element_size().y};
}

[[nodiscard]] Point GridView::element_size() const
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
    down_index_ = grid_index({event.x, event.y});
    selected_index_.reset();
}

void GridView::on_button_up(const SDL_MouseButtonEvent& event)
{
    const Point up_index = grid_index({event.x, event.y});
    if (sdl::point_equals(up_index, down_index_)) {
        selected_index_ = up_index;
        if (on_cell_clicked_) {
            (*on_cell_clicked_)(up_index);
        }
    }
}
