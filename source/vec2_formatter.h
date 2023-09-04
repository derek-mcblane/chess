#pragma once

#include "vec2.h"

#include <spdlog/fmt/fmt.h>

template <typename T>
struct fmt::formatter<dm::Vec2<T>> : fmt::formatter<std::string>
{
    auto format(dm::Vec2<T> point, format_context& ctx) -> decltype(ctx.out())
    {
        return fmt::format_to(ctx.out(), "[Position x={}, y={}]", point.x(), point.y());
    }
};
