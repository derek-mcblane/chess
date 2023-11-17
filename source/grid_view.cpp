#include "grid_view.h"

#include "sdlpp.h"

#include <spdlog/spdlog.h>

sdl::Rectangle<int> ClickableGrid::grid_cell(sdl::Point<int> index) const
{
    using namespace sdl::rectangle_operators;
    return grid_cell_local(index) + origin();
}

sdl::Rectangle<int> ClickableGrid::grid_cell_local(sdl::Point<int> index) const
{
    const auto width = cell_size().x;
    const auto height = cell_size().y;
    return {index.x * width, index.y * height, width, height};
}

sdl::Point<int> ClickableGrid::grid_index(sdl::Point<int> position) const
{
    using namespace sdl::point_operators;
    return grid_index_local(position - origin());
}

sdl::Point<int> ClickableGrid::grid_index_local(const sdl::Point<int> position) const
{
    return {position.x / cell_size().x, position.y / cell_size().y};
}

sdl::Point<int> ClickableGrid::grid_cell_position(sdl::Point<int> index) const
{
    using namespace sdl::point_operators;
    return grid_cell_position_local(index) + origin();
}

sdl::Point<int> ClickableGrid::grid_cell_position_local(sdl::Point<int> index) const
{
    return {index.x * cell_size().x, index.y * cell_size().y};
}

sdl::Point<int> ClickableGrid::cell_size() const
{
    return {size().x / grid_size.x, size().y / grid_size.y};
}

sdl::Point<float> ClickableGrid::cell_size_f() const
{
    return {
        static_cast<float>(size().x) / static_cast<float>(grid_size.x),
        static_cast<float>(size().y) / static_cast<float>(grid_size.y)};
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
    const auto click_position = sdl::Point<int>{event.x, event.y};
    sdl::Point<int> clicked_index{grid_index(click_position)};
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
    const sdl::Point<int> up_index = grid_index({event.x, event.y});
    if (up_index == down_index_.load()) {
        clicked_index_ = up_index;
        if (on_cell_clicked_) {
            (*on_cell_clicked_)(up_index);
        }
    }
}

sdl::Rectangle<int>& ClickableGrid::region_impl()
{
    return region_;
}
const sdl::Rectangle<int>& ClickableGrid::region_impl() const
{
    return region_;
}

void ClickableGrid::draw_impl(sdl::Renderer& renderer) const {

};
