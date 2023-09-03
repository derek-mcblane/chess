#include "sdlpp.h"

#include <map>

template <typename T>
class SpriteGrid
{
  public:
    using Point = sdl::Point<int>;
    using CoordinateMap = std::map<T, Point>;

    SpriteGrid() = default;

    SpriteGrid(Point grid_size, CoordinateMap&& sprite_coordinates)
        : coordinates_{std::move(sprite_coordinates)}, grid_size_{grid_size}
    {}

    [[nodiscard]] const sdl::Texture& texture() const
    {
        return texture_;
    }

    [[nodiscard]] sdl::Texture& texture()
    {
        return texture_;
    }

    [[nodiscard]] sdl::Rectangle<int> get_region(const T& sprite);

  private:
    sdl::Texture texture_;
    CoordinateMap coordinates_;
    Point grid_size_;

    [[nodiscard]] sdl::Rectangle<int> get_region(Point coordinate) const;
    [[nodiscard]] Point pitch() const;
};

template <typename T>
sdl::Rectangle<int> SpriteGrid<T>::get_region(Point coordinate) const
{
    return {.x = coordinate.x * pitch().x, .y = coordinate.y * pitch().y, .w = pitch().x, .h = pitch().y};
}

template <typename T>
sdl::Rectangle<int> SpriteGrid<T>::get_region(const T& sprite)
{
    return get_region(coordinates_.at(sprite));
}

template <typename T>
SpriteGrid<T>::Point SpriteGrid<T>::pitch() const
{
    const auto texture_size = texture_.size();
    return {texture_size.x / grid_size_.x, texture_size.y / grid_size_.y};
}
