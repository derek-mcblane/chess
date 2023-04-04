#include "bit_board.h"
#include "vec2.h"

#include <map>
#include <optional>

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
    void clear_all();
    [[nodiscard]] bool occupied(const Position& position) const;
    [[nodiscard]] PieceColor piece_color_at(const Position& position) const;
    [[nodiscard]] PieceType piece_type_at(const Position& position) const;
    [[nodiscard]] Piece occupant_at(const Position& position) const;
    [[nodiscard]] bool is_valid_move(const Position& from, const Position& to);
    [[nodiscard]] BitBoard valid_moves(const Position& from);

    [[nodiscard]] static BoardPieces make_standard_setup_board();

  private:
    inline static const Position::dimension_type black_piece_row{0};
    inline static const Position::dimension_type black_pawn_row{1};
    inline static const Position black_king_position{0, 4};
    inline static const Position black_queenside_rook_position{0, 0};
    inline static const Position black_kingside_rook_position{0, 7};
    inline static const Position::dimension_type white_piece_row{7};
    inline static const Position::dimension_type white_pawn_row{6};
    inline static const Position white_king_position{7, 4};
    inline static const Position white_queenside_rook_position{7, 0};
    inline static const Position white_kingside_rook_position{7, 7};

    [[nodiscard]] bool occupied(BitBoard position) const;
    [[nodiscard]] PieceColor piece_color_at(const BitBoard& position) const;
    void clear_pieces(BitBoard board);
    void set_pieces(const Piece& piece, BitBoard positions);
    void set_squares_attacked_by(const Position& position);
    void clear_squares_attacked_by(const Position& position);
    void update_en_passant_state(const Position& from, const Position& to);
    void update_castling_state(const Position& from, const Position& to);

    [[nodiscard]] BitBoard pawn_moves(const Position& from) const;
    [[nodiscard]] BitBoard knight_moves(const Position& from) const;
    [[nodiscard]] BitBoard bishop_moves(const Position& from) const;
    [[nodiscard]] BitBoard rook_moves(const Position& from) const;
    [[nodiscard]] BitBoard queen_moves(const Position& from) const;
    [[nodiscard]] BitBoard king_moves(const Position& from) const;
    [[nodiscard]] BitBoard valid_moves_bitboard(const Position& from) const;

    template <Direction D>
    [[nodiscard]] BitBoard sliding_moves(const Position& from, const size_t range = BitBoard::board_size) const
    {
        BitBoard candidates{from};
        BitBoard prev_candidates{candidates};
        for (size_t distance = 0; !candidates.on_edge<D>() && distance < range; ++distance) {
            prev_candidates = candidates;
            candidates.dilate<D>();

            const BitBoard candidate{prev_candidates ^ candidates};
            if (occupied(candidate)) {
                if (piece_color_at(from) == piece_color_at(candidate)) {
                    candidates.reset(candidate);
                }
                break;
            }
        }
        return candidates.reset(from);
    }

    [[nodiscard]] BitBoard
    sliding_moves(const Position& from, Direction direction, size_t range = BitBoard::board_size) const;

    template <typename DirectionRange>
    [[nodiscard]] BitBoard
    sliding_moves(const Position& from, DirectionRange&& directions, size_t range = BitBoard::board_size) const
    {
        BitBoard moves;
        for (const auto direction : std::forward<DirectionRange>(directions)) {
            moves.set(sliding_moves(from, direction, range));
        }
        return moves.reset(from);
    }

    [[nodiscard]] static bool on_black_pawn_start_square(const Position& from);
    [[nodiscard]] static bool on_white_pawn_start_square(const Position& from);
    [[nodiscard]] bool on_pawn_start_square(const Position& from) const;
    void remove_if_color(BitBoard& moves, const PieceColor& color) const;

    BitBoard occupied_;
    BitBoard black_;
    BitBoard white_;
    BitBoard attacked_by_black_;
    BitBoard attacked_by_white_;
    BitBoard en_passant_square_;
    BitBoard pawns_;
    BitBoard knights_;
    BitBoard bishops_;
    BitBoard rooks_;
    BitBoard queens_;
    BitBoard kings_;
    bool black_king_moved_{false};
    bool black_queenside_rook_moved_{false};
    bool black_kingside_rook_moved_{false};
    bool white_king_moved_{false};
    bool white_queenside_rook_moved_{false};
    bool white_kingside_rook_moved_{false};
};

} // namespace chess
