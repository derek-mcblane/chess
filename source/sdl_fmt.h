#include "sdlpp.h"

#include <spdlog/fmt/fmt.h>

template <>
struct fmt::formatter<SDL_MouseButtonEvent> : fmt::formatter<std::string>
{
    auto format(SDL_MouseButtonEvent event, format_context& ctx) -> decltype(ctx.out())
    {
        return fmt::format_to(
            ctx.out(),
            "[SDL_MouseButtonEvent"
            "type={}, timestamp={}, windowID={}, which={}, button={}, "
            "state={}, clicks={}, padding1={}, x={}, y={}]",
            event.type,
            event.timestamp,
            event.windowID,
            event.which,
            event.button,
            event.state,
            event.clicks,
            event.padding1,
            event.x,
            event.y
        );
    }
};

template <sdl::PointT Point>
struct fmt::formatter<Point> : fmt::formatter<std::string>
{
    auto format(Point point, format_context& ctx) -> decltype(ctx.out())
    {
        return fmt::format_to(ctx.out(), "[Point x={}, y={}]", point.x, point.y);
    }
};
