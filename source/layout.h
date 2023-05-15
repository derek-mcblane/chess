#pragma once

#include "sdlpp.h"

template <typename Rectangle>
inline Rectangle remove_margin(Rectangle original, sdl::rectangle_dimension_type<Rectangle> padding)
{
    return {original.x - padding, original.y - padding, original.w - 2 * padding, original.h - 2 * padding};
}
