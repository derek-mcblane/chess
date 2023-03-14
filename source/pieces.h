#include "bit_board.h"
#include "vec2.h"

#include <map>

namespace chess {

enum class PieceColor : int
{
    black,
    white,
    none,
};
extern const std::map<PieceColor, std::string> piece_color_names;

enum class PieceType : int
{
    pawn,
    knight,
    bishop,
    rook,
    queen,
    king,
    none,
};
extern const std::map<PieceType, std::string> piece_type_names;

struct Piece
{
    PieceColor color;
    PieceType type;
};

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

inline constexpr Piece null_piece{.color = PieceColor::none, .type = PieceType::none};

class Pieces
{
  public:
    using Coord = dm::Coord<int>;

    void set_piece(const Piece& piece, const Coord& position);
    void clear_piece(const Coord& position);

    [[nodiscard]] bool occupied(const Coord& position) const;
    [[nodiscard]] PieceColor piece_color_at(const Coord& position) const;
    [[nodiscard]] PieceType piece_type_at(const Coord& position) const;
    [[nodiscard]] Piece occupant_at(const Coord& position) const;

  private:
    BitBoard occupied_;
    BitBoard white_;
    BitBoard black_;
    BitBoard pawns_;
    BitBoard knights_;
    BitBoard bishops_;
    BitBoard rooks_;
    BitBoard queens_;
    BitBoard kings_;
};

} // namespace chess
