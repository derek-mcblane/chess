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

[[nodiscard]] PieceType next_piece_type(const PieceType type);
[[nodiscard]] PieceColor next_piece_color(const PieceColor color);
[[nodiscard]] PieceColor toggle_piece_color(const PieceColor color);

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

class BoardPieces
{
  public:
    using Coord = dm::Coord<int>;

    void set_piece(const Piece& piece, const Coord& position);
    void move_piece(const Coord& from, const Coord& to);
    void clear_piece(const Coord& position);
    [[nodiscard]] bool occupied(const Coord& position) const;
    [[nodiscard]] PieceColor piece_color_at(const Coord& position) const;
    [[nodiscard]] PieceType piece_type_at(const Coord& position) const;
    [[nodiscard]] Piece occupant_at(const Coord& position) const;
    [[nodiscard]] bool is_valid_move(const Coord& from, const Coord& to);
    [[nodiscard]] BitBoard valid_moves(const Coord& from);

    [[nodiscard]] static BoardPieces make_standard_setup_board();

  private:
    [[nodiscard]] BitBoard pawn_moves(const Coord& from);
    [[nodiscard]] BitBoard knight_moves(const Coord& from);
    [[nodiscard]] BitBoard bishop_moves(const Coord& from);
    [[nodiscard]] BitBoard rook_moves(const Coord& from);
    [[nodiscard]] BitBoard queen_moves(const Coord& from);
    [[nodiscard]] BitBoard king_moves(const Coord& from);
    [[nodiscard]] BitBoard valid_moves_bitboard(const Coord& from);

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
