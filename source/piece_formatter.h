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

template <>
struct fmt::formatter<chess::PieceType> : fmt::formatter<std::string>
{
    auto format(chess::PieceType type, format_context& ctx) -> decltype(ctx.out())
    {
        return fmt::format_to(ctx.out(), "{}", chess::piece_type_names.at(type));
    }
};

template <>
struct fmt::formatter<chess::PieceColor> : fmt::formatter<std::string>
{
    auto format(chess::PieceColor color, format_context& ctx) -> decltype(ctx.out())
    {
        return fmt::format_to(ctx.out(), "{}", chess::piece_color_names.at(color));
    }
};
