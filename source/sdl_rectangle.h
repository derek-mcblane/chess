#include "sdlpp.h"

template <sdl::PointT Point>
sdl::Rectangle<sdl::point_dimension_type<Point>> make_rectangle(const Point origin, const Point size)
{
    return {origin.x, origin.y, size.x, size.y};
}

template <sdl::RectangleT Rectangle>
sdl::Point<sdl::rectangle_dimension_type<Rectangle>> rectangle_center(const Rectangle rectangle)
{
    return {rectangle.x + rectangle.w / 2, rectangle.y + rectangle.h / 2};
}

template <sdl::RectangleT Rectangle>
sdl::Point<float> rectangle_center_f(const Rectangle rectangle)
{
    return {rectangle.x + rectangle.w / 2.0F, rectangle.y + rectangle.h / 2.0F};
}

template <sdl::RectangleT Rectangle>
sdl::Point<sdl::rectangle_dimension_type<Rectangle>> rectangle_origin(const Rectangle rectangle)
{
    return {rectangle.x, rectangle.y};
}

template <sdl::RectangleT Rectangle>
sdl::Point<sdl::rectangle_dimension_type<Rectangle>> rectangle_size(const Rectangle rectangle)
{
    return {rectangle.w, rectangle.h};
}

template <sdl::RectangleT Rectangle>
sdl::rectangle_dimension_type<Rectangle> rectangle_area(const Rectangle rectangle)
{
    return rectangle.w * rectangle.h;
}
