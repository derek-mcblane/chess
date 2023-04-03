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

class BoardPieces
{
  public:
    using Position = BitBoard::Position;

    void set_piece(const Piece& piece, const Position& position);
    void move_piece(const Position& from, const Position& to);
    void clear_piece(const Position& position);
    [[nodiscard]] bool occupied(const Position& position) const;
    [[nodiscard]] PieceColor piece_color_at(const Position& position) const;
    [[nodiscard]] PieceType piece_type_at(const Position& position) const;
    [[nodiscard]] Piece occupant_at(const Position& position) const;
    [[nodiscard]] bool is_valid_move(const Position& from, const Position& to);
    [[nodiscard]] BitBoard valid_moves(const Position& from);

    [[nodiscard]] static BoardPieces make_standard_setup_board();

  private:
    [[nodiscard]] bool occupied(BitBoard position) const;
    [[nodiscard]] PieceColor piece_color_at(const BitBoard& position) const;

    [[nodiscard]] BitBoard pawn_moves(const Position& from) const;
    [[nodiscard]] BitBoard knight_moves(const Position& from) const;
    [[nodiscard]] BitBoard bishop_moves(const Position& from) const;
    [[nodiscard]] BitBoard rook_moves(const Position& from) const;
    [[nodiscard]] BitBoard queen_moves(const Position& from) const;
    [[nodiscard]] BitBoard king_moves(const Position& from) const;
    [[nodiscard]] BitBoard valid_moves_bitboard(const Position& from) const;

    template <typename DirectionRange>
    [[nodiscard]] BitBoard sliding_moves(const Position& from, DirectionRange&& directions) const
    {
        BitBoard moves;
        for (const auto direction : std::forward<DirectionRange>(directions)) {
            BitBoard candidates{from};
            BitBoard prev_candidates{candidates};
            while (!candidates.on_edge(direction)) {
                prev_candidates = candidates;
                candidates.dilate(direction);

                const BitBoard candidate{prev_candidates ^ candidates};
                if (occupied(candidate)) {
                    if (piece_color_at(from) == piece_color_at(candidate)) {
                        candidates.reset(candidate);
                    }
                    break;
                }
            }
            moves.set(candidates);
        }
        return moves.reset(from);
    }

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
