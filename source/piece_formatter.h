#pragma once

#include "pieces.h"

#include <spdlog/spdlog.h>

template <>
struct fmt::formatter<chess::Piece> : fmt::formatter<std::string>
{
    auto format(chess::Piece piece, format_context& ctx) -> decltype(ctx.out())
    {
        return fmt::format_to(
            ctx.out(), "{} {}", chess::piece_color_names.at(piece.color), chess::piece_type_names.at(piece.type)
        );
    }
};

