#pragma once

#include "sdlpp.h"

#define IM_VEC2_CLASS_EXTRA                                                                                            \
    constexpr ImVec2(const sdl::Point<float>& f) : x(f.x), y(f.y) {}                                                   \
    operator sdl::Point<float>() const                                                                                 \
    {                                                                                                                  \
        return {x, y};                                                                                                 \
    }
