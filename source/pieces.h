#pragma once

#include "vec2.h"

#include <cassert>

#include <exception>
#include <map>
#include <optional>
#include <set>

namespace chess {

enum class PieceColor : int
{
    black,
    white,
};
extern const std::map<PieceColor, std::string> piece_color_names;
extern const std::map<PieceColor, std::string> piece_color_short_names;

template <PieceColor Color>
struct OppositeColor
{
    static const PieceColor value;
};

template <>
inline const PieceColor OppositeColor<PieceColor::black>::value = PieceColor::white;
template <>
inline const PieceColor OppositeColor<PieceColor::white>::value = PieceColor::black;

template <PieceColor Color>
inline constexpr PieceColor opposite_color_v = OppositeColor<Color>::value;

inline PieceColor opposite_color(const PieceColor color)
{
    return (color == PieceColor::black) ? PieceColor::white : PieceColor::black;
}

enum class PieceType : int
{
    pawn,
    knight,
    bishop,
    rook,
    queen,
    king,
};
extern const std::map<PieceType, std::string> piece_type_names;
extern const std::map<PieceType, std::string> piece_type_short_names;

struct Piece
{
    PieceColor color;
    PieceType type;
};

namespace pieces {
constexpr auto black_pawn = Piece{PieceColor::black, PieceType::pawn};
constexpr auto black_knight = Piece{PieceColor::black, PieceType::knight};
constexpr auto black_bishop = Piece{PieceColor::black, PieceType::bishop};
constexpr auto black_rook = Piece{PieceColor::black, PieceType::rook};
constexpr auto black_queen = Piece{PieceColor::black, PieceType::queen};
constexpr auto black_king = Piece{PieceColor::black, PieceType::king};
constexpr auto white_pawn = Piece{PieceColor::white, PieceType::pawn};
constexpr auto white_knight = Piece{PieceColor::white, PieceType::knight};
constexpr auto white_bishop = Piece{PieceColor::white, PieceType::bishop};
constexpr auto white_rook = Piece{PieceColor::white, PieceType::rook};
constexpr auto white_queen = Piece{PieceColor::white, PieceType::queen};
constexpr auto white_king = Piece{PieceColor::white, PieceType::king};
} // namespace pieces

inline bool operator==(const Piece& lhs, const Piece& rhs)
{
    return (lhs.type == rhs.type) && (lhs.color == rhs.color);
}

inline bool operator!=(const Piece& lhs, const Piece& rhs)
{
    return !(lhs == rhs);
}

inline bool operator<(const Piece& lhs, const Piece& rhs)
{
    const auto lhs_type = static_cast<std::underlying_type_t<PieceType>>(lhs.type);
    const auto lhs_color = static_cast<std::underlying_type_t<PieceColor>>(lhs.color);
    const auto rhs_type = static_cast<std::underlying_type_t<PieceType>>(rhs.type);
    const auto rhs_color = static_cast<std::underlying_type_t<PieceColor>>(rhs.color);

    if (lhs_type == rhs_type) {
        return lhs_color < rhs_color;
    }
    return lhs_type < rhs_type;
}

inline bool operator>=(const Piece& lhs, const Piece& rhs)
{
    return !(rhs < lhs);
}

inline bool operator>(const Piece& lhs, const Piece& rhs)
{
    return rhs < lhs;
}

inline bool operator<=(const Piece& lhs, const Piece& rhs)
{
    return !(rhs > lhs);
}

} // namespace chess
