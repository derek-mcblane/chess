#include "sdlpp.h"

#include <gsl/util>

inline sdl::Point<int> make_point(float x, float y)
{
    return sdl::Point<int>{gsl::narrow_cast<int>(std::ceil(x)), gsl::narrow_cast<int>(std::ceil(y))};
}

template <sdl::PointT Point>
sdl::point_dimension_type<Point> size_area(Point size)
{
    return size.x * size.y;
}
