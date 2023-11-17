#pragma once

#include "sdlpp.h"
#include "widget.h"

#include <atomic>
#include <functional>

class ClickableGrid : public Widget
{
  public:
    using OnClickedCallback = std::function<void(const sdl::Point<int>&)>;

    ClickableGrid(sdl::Point<int> grid_size, sdl::Rectangle<int> region) : grid_size(grid_size), region_(region) {}

    sdl::Point<int> grid_size;

    [[nodiscard]] sdl::Rectangle<int> grid_cell_local(sdl::Point<int> index) const;
    [[nodiscard]] sdl::Rectangle<int> grid_cell(sdl::Point<int> index) const;
    [[nodiscard]] sdl::Point<int> grid_index_local(sdl::Point<int> position) const;
    [[nodiscard]] sdl::Point<int> grid_index(sdl::Point<int> position) const;
    [[nodiscard]] sdl::Point<int> grid_cell_position_local(sdl::Point<int> index) const;
    [[nodiscard]] sdl::Point<int> grid_cell_position(sdl::Point<int> index) const;
    [[nodiscard]] sdl::Point<int> cell_size() const;
    [[nodiscard]] sdl::Point<float> cell_size_f() const;

    std::optional<sdl::Point<int>> clicked_index() const
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
    sdl::Rectangle<int> region_;
    sdl::Texture texture_;
    std::atomic<sdl::Point<int>> down_index_;
    std::optional<std::atomic<sdl::Point<int>>> clicked_index_;
    std::optional<OnClickedCallback> on_cell_clicked_;

    void on_button_down_impl(const SDL_MouseButtonEvent& event) override;
    void on_button_up_impl(const SDL_MouseButtonEvent& event) override;
    sdl::Rectangle<int>& region_impl() override;
    [[nodiscard]] const sdl::Rectangle<int>& region_impl() const override;
    void draw_impl(sdl::Renderer& renderer) const override;
};
