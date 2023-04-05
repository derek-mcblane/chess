#include "bit_board.h"
#include "vec2.h"

#include <cassert>
#include <map>
#include <optional>

namespace chess {

enum class PieceColor : int
{
    black,
    white,
};
extern const std::map<PieceColor, std::string> piece_color_names;

inline PieceColor opposite_color(const PieceColor color)
{
    switch (color) {
    case PieceColor::black:
        return PieceColor::white;
    case PieceColor::white:
        return PieceColor::black;
    }
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

class BoardPieces
{
  public:
    using Position = BitBoard::Position;

    void set_piece(Piece piece, const Position& position);
    void move_piece(Position from, Position to);
    void clear_piece(const Position& position);
    [[nodiscard]] bool occupied(const Position& position) const;
    [[nodiscard]] std::optional<PieceColor> piece_color_at(const Position& position) const;
    [[nodiscard]] std::optional<PieceType> piece_type_at(const Position& position) const;
    [[nodiscard]] std::optional<Piece> piece_at(const Position& position) const;
    [[nodiscard]] bool is_valid_move(Position from, Position to);
    [[nodiscard]] BitBoard valid_moves(Position from);
    [[nodiscard]] PieceColor active_color() const;
    [[nodiscard]] bool is_active_piece(const Position& position) const;

    [[nodiscard]] static BoardPieces make_standard_setup_board();

  private:
    struct Move
    {
        Piece piece;
        Position from;
        Position to;
    };

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
    inline static const Position::dimension_type kingside_rook_col{0};
    inline static const Position::dimension_type queenside_rook_col{7};

    [[nodiscard]] bool occupied(BitBoard position) const;
    [[nodiscard]] BitBoard occupied_board() const;
    [[nodiscard]] BitBoard attacked_board() const;
    [[nodiscard]] std::optional<PieceType> piece_type_at(BitBoard position) const;
    [[nodiscard]] bool is_pawn(BitBoard position) const;
    [[nodiscard]] bool is_knight(BitBoard position) const;
    [[nodiscard]] bool is_bishop(BitBoard position) const;
    [[nodiscard]] bool is_rook(BitBoard position) const;
    [[nodiscard]] bool is_queen(BitBoard position) const;
    [[nodiscard]] bool is_king(BitBoard position) const;
    [[nodiscard]] std::optional<PieceColor> piece_color_at(BitBoard position) const;
    [[nodiscard]] bool is_black(BitBoard position) const;
    [[nodiscard]] bool is_white(BitBoard position) const;
    void clear_pieces(BitBoard board);
    void set_pieces(Piece piece, BitBoard positions);
    void set_squares_attacked_by(const Position& position);
    void clear_squares_attacked_by(const Position& position);
    void update_en_passant_state(Move move);
    void update_castling_state(Move move);

    void move_pawn(BitBoard from, BitBoard to);
    void move_knight(BitBoard from, BitBoard to);
    void move_bishop(BitBoard from, BitBoard to);
    void move_rook(BitBoard from, BitBoard to);
    void move_queen(BitBoard from, BitBoard to);
    void move_king(BitBoard from, BitBoard to);

    [[nodiscard]] BitBoard pawn_moves(Position from) const;
    [[nodiscard]] BitBoard knight_moves(Position from) const;
    [[nodiscard]] BitBoard bishop_moves(Position from) const;
    [[nodiscard]] BitBoard rook_moves(Position from) const;
    [[nodiscard]] BitBoard queen_moves(Position from) const;
    template <std::uint64_t Between, std::uint64_t KingSquares, std::uint64_t Move>
    [[nodiscard]] BitBoard king_castling_move(BitBoard from) const;
    [[nodiscard]] BitBoard king_castling_moves(Position from) const;
    [[nodiscard]] BitBoard king_moves(Position from) const;
    [[nodiscard]] BitBoard valid_moves_bitboard(Position from) const;

    template <Direction D>
    [[nodiscard]] BitBoard sliding_moves(BitBoard from, size_t range = BitBoard::board_size) const;
    [[nodiscard]] BitBoard sliding_moves(BitBoard from, Direction direction, size_t range = BitBoard::board_size) const;
    template <typename DirectionRange>
    [[nodiscard]] BitBoard
    sliding_moves(Position from, DirectionRange&& directions, size_t range = BitBoard::board_size) const;

    [[nodiscard]] bool is_pawn_start_square(Position from) const;
    void remove_if_color(BitBoard& moves, PieceColor color) const;
    [[nodiscard]] BitBoard board_of_color(PieceColor color) const;
    [[nodiscard]] BitBoard& board_of_color(PieceColor color);
    [[nodiscard]] BitBoard active_color_board() const;
    [[nodiscard]] BitBoard& active_color_board();
    [[nodiscard]] BitBoard inactive_color_board() const;
    [[nodiscard]] BitBoard& inactive_color_board();

    std::vector<Move> move_history_;
    std::map<Position, BitBoard> attacked_by_;
    BitBoard attacked_;
    BitBoard en_passant_square_;
    BitBoard pawns_;
    BitBoard knights_;
    BitBoard bishops_;
    BitBoard rooks_;
    BitBoard queens_;
    BitBoard kings_;
    BitBoard black_;
    BitBoard white_;
    PieceColor active_color_{PieceColor::white};
    bool black_queenside_castle_piece_moved_{false};
    bool black_kingside_castle_piece_moved_{false};
    bool white_queenside_castle_piece_moved_{false};
    bool white_kingside_castle_piece_moved_{false};
};

template <Direction D>
[[nodiscard]] BitBoard BoardPieces::sliding_moves(const BitBoard from, const size_t range) const
{
    const auto from_color = piece_color_at(from);
    if (!from_color.has_value()) {
        assert(!"sliding moves from empty square");
        return BitBoard{};
    }
    auto moves = from;
    for (size_t distance = 0; distance < range; ++distance) {
        moves.dilate<D>();
        if ((moves & ~from).test_any(occupied_board()) || moves.on_edge<D>()) {
            break;
        }
    }
    return moves.clear(board_of_color(*from_color));
}

template <typename DirectionRange>
[[nodiscard]] BitBoard BoardPieces::sliding_moves(const Position from, DirectionRange&& directions, size_t range) const
{
    BitBoard moves;
    for (const auto direction : std::forward<DirectionRange>(directions)) {
        moves.set(sliding_moves(BitBoard{from}, direction, range));
    }
    return moves.clear(from);
}

template <std::uint64_t Between, std::uint64_t KingSquares, std::uint64_t Move>
[[nodiscard]] BitBoard BoardPieces::king_castling_move(const BitBoard from) const
{
    static constexpr BitBoard between{Between};
    static constexpr BitBoard king_squares{KingSquares};
    static constexpr BitBoard move{Move};

    if (between.test_any(occupied_board()) || king_squares.test_any(attacked_board())) {
        return BitBoard{};
    }
    return move;
}

} // namespace chess
