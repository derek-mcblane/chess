#include "grid_view.h"

#include "sdlpp.h"

#include <spdlog/spdlog.h>

sdl::Rectangle<int> ClickableGrid::grid_cell(ClickableGrid::Point index) const
{
    using namespace sdl::rectangle_operators;
    return grid_cell_local(index) + origin();
}

sdl::Rectangle<int> ClickableGrid::grid_cell_local(ClickableGrid::Point index) const
{
    const auto width = cell_size().x;
    const auto height = cell_size().y;
    return {index.x * width, index.y * height, width, height};
}

ClickableGrid::Point ClickableGrid::grid_index(ClickableGrid::Point position) const
{
    using namespace sdl::point_operators;
    return grid_index_local(position - origin());
}

ClickableGrid::Point ClickableGrid::grid_index_local(const ClickableGrid::Point position) const
{
    return {position.x / cell_size().x, position.y / cell_size().y};
}

ClickableGrid::Point ClickableGrid::grid_cell_position(ClickableGrid::Point index) const
{
    using namespace sdl::point_operators;
    return grid_cell_position_local(index) + origin();
}

ClickableGrid::Point ClickableGrid::grid_cell_position_local(ClickableGrid::Point index) const
{
    return {index.x * cell_size().x, index.y * cell_size().y};
}

ClickableGrid::Point ClickableGrid::cell_size() const
{
    return {size().x / grid_size.x, size().y / grid_size.y};
}

void ClickableGrid::set_on_cell_clicked_callback(OnClickedCallback&& callback)
{
    on_cell_clicked_ = std::move(callback);
}

void ClickableGrid::clear_on_cell_clicked_callback()
{
    on_cell_clicked_.reset();
}

void ClickableGrid::on_button_down_impl(const SDL_MouseButtonEvent& event)
{
    using namespace sdl::point_operators;
    clicked_index_.reset();
    const auto click_position = ClickableGrid::Point{event.x, event.y};
    ClickableGrid::Point clicked_index{grid_index(click_position)};
    if (clicked_index.x < 0 || clicked_index.x >= grid_size.x) {
        return;
    }
    if (clicked_index.y < 0 || clicked_index.y >= grid_size.y) {
        return;
    }
    down_index_ = clicked_index;
}

void ClickableGrid::on_button_up_impl(const SDL_MouseButtonEvent& event)
{
    using namespace sdl::point_operators;
    const ClickableGrid::Point up_index = grid_index({event.x, event.y});
    if (up_index == down_index_.load()) {
        clicked_index_ = up_index;
        if (on_cell_clicked_) {
            (*on_cell_clicked_)(up_index);
        }
    }
}

ClickableGrid::Region& ClickableGrid::region_impl()
{
    return region_;
}
const ClickableGrid::Region& ClickableGrid::region_impl() const
{
    return region_;
}

void ClickableGrid::draw_impl(sdl::Renderer& renderer) const {

};
