#include "sdlpp.h"

#include <map>

class SpriteMapGrid
{
    using Point = sdl::Point<int>;

  public:
    SpriteMapGrid(Point map_size, Point n_elements) : pitch_{map_size.x / n_elements.x, map_size.y / n_elements.y} {}

    [[nodiscard]] sdl::Rectangle<int> get_region(Point coordinate) const
    {
        return {.x = coordinate.x * pitch_.x, .y = coordinate.y * pitch_.y, .w = pitch_.x, .h = pitch_.y};
    }

  private:
    Point pitch_;
};

template <typename T>
class SpriteGrid
{
  public:
    using Point = sdl::Point<int>;
    using CoordinateMap = std::map<T, Point>;

    SpriteGrid(Point texture_size, Point grid_size, CoordinateMap&& sprite_coordinates)
        : pitch_{texture_size.x / grid_size.x, texture_size.y / grid_size.y},
          coordinates_{std::move(sprite_coordinates)}
    {}

    [[nodiscard]] sdl::Rectangle<int> get_region(const T& sprite);

  private:
    Point pitch_;
    CoordinateMap coordinates_;

    [[nodiscard]] sdl::Rectangle<int> get_region(Point coordinate) const;
};
