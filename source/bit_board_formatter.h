#include "bit_board.h"

#include <spdlog/spdlog.h>

template <>
struct fmt::formatter<BitBoard> : fmt::formatter<std::string>
{
    auto format(const BitBoard bit_board, format_context& ctx) -> decltype(ctx.out())
    {
        return format_to(ctx.out(), "[BitBoard {}]", bit_board.to_string());
    }
};
