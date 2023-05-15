#pragma once

#include "vec2.h"
#include "bit_board.h"
#include "pieces.h"

#include <optional>
#include <set>

namespace chess {

class Board
{
  public:
    using Position = BitBoard::Position;

    struct Move {
        Position from;
        Position to;
    };

    struct PromotionMove {
        Move move;
        std::optional<PieceType> promotion;
    };

    void set_piece(Piece piece, const Position& position);
    void make_move(Move move);
    void promote(PromotionMove move);
    void undo_previous_move();
    void clear_piece(const Position& position);
    [[nodiscard]] bool occupied(const Position& position) const;
    [[nodiscard]] std::optional<PieceColor> piece_color_at(const Position& position) const;
    [[nodiscard]] std::optional<PieceType> piece_type_at(const Position& position) const;
    [[nodiscard]] std::optional<Piece> piece_at(const Position& position) const;
    [[nodiscard]] bool is_valid_move(Move move) const;
    [[nodiscard]] bool is_promotion_move(Move move) const;
    [[nodiscard]] std::vector<Board::Position> valid_moves_vector(Position from);
    [[nodiscard]] std::set<Board::Position> valid_moves_set(Position from);
    [[nodiscard]] PieceColor active_color() const;
    [[nodiscard]] bool is_active_piece(const Position& position) const;
    [[nodiscard]] std::vector<Board::Position> attacked_by_white() const;
    [[nodiscard]] std::vector<Board::Position> attacked_by_black() const;

    [[nodiscard]] static Board make_standard_setup_board();

  private:
    struct BitBoardMove
    {
        BitBoard from;
        BitBoard to;
    };

    struct BitBoardPieceMove
    {
        Piece piece;
        BitBoard from;
        BitBoard to;
    };

    inline static constexpr Position::dimension_type black_piece_row{0};
    inline static constexpr Position::dimension_type black_pawn_row{1};
    inline static constexpr Position::dimension_type white_piece_row{7};
    inline static constexpr Position::dimension_type white_pawn_row{6};

    inline static constexpr BitBoard black_king_position{BitBoard::Position{0, 4}};
    inline static constexpr BitBoard black_kingside_rook_position{BitBoard::Position{0, 7}};
    inline static constexpr BitBoard black_queenside_rook_position{BitBoard::Position{0, 0}};
    inline static constexpr BitBoard black_castle_kingside_king_move{BitBoard::Position{0, 6}};
    inline static constexpr BitBoard black_castle_queenside_king_move{BitBoard::Position{0, 2}};
    inline static constexpr BitBoard black_castle_kingside_rook_move{BitBoard::Position{0, 5}};
    inline static constexpr BitBoard black_castle_queenside_rook_move{BitBoard::Position{0, 3}};

    inline static constexpr BitBoard white_king_position{BitBoard::Position{7, 4}};
    inline static constexpr BitBoard white_kingside_rook_position{BitBoard::Position{7, 7}};
    inline static constexpr BitBoard white_queenside_rook_position{BitBoard::Position{7, 0}};
    inline static constexpr BitBoard white_castle_kingside_king_move{BitBoard::Position{7, 6}};
    inline static constexpr BitBoard white_castle_queenside_king_move{BitBoard::Position{7, 2}};
    inline static constexpr BitBoard white_castle_kingside_rook_move{BitBoard::Position{7, 5}};
    inline static constexpr BitBoard white_castle_queenside_rook_move{BitBoard::Position{7, 3}};

    std::vector<BitBoardPieceMove> move_history_;
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

    [[nodiscard]] bool occupied(BitBoard position) const;
    [[nodiscard]] std::optional<Piece> piece_at(BitBoard position) const;
    [[nodiscard]] Piece piece_at_checked(BitBoard position) const;
    [[nodiscard]] BitBoard occupied_board() const;
    [[nodiscard]] BitBoard attacked_by_white_board() const;
    [[nodiscard]] BitBoard attacked_by_black_board() const;
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
    void move_piece(BitBoardPieceMove move);
    void promote_piece(BitBoardMove move);
    void make_move(BitBoardPieceMove move);
    void castle(BitBoardPieceMove king_move);
    void white_castle(BitBoardPieceMove king_move);
    void black_castle(BitBoardPieceMove king_move);
    void update_en_passant_state(BitBoardPieceMove move);
    void update_castling_state(BitBoardPieceMove move);

    void move_pawn(BitBoardMove move);
    void move_knight(BitBoardMove move);
    void move_bishop(BitBoardMove move);
    void move_rook(BitBoardMove move);
    void move_queen(BitBoardMove move);
    void move_king(BitBoardMove move);
    void move_black(BitBoardMove move);
    void move_white(BitBoardMove move);

    [[nodiscard]] BitBoard valid_moves_bitboard(Position from) const;
    [[nodiscard]] BitBoard attacking_bitboard(Position from) const;
    [[nodiscard]] BitBoard pawn_moves(Position from, PieceColor color) const;
    [[nodiscard]] BitBoard pawn_attacking_squares(Position from, PieceColor color) const;
    [[nodiscard]] BitBoard pawn_attacking_moves(Position from, PieceColor color) const;
    [[nodiscard]] BitBoard knight_moves(Position from, PieceColor color) const;
    [[nodiscard]] BitBoard bishop_moves(Position from, PieceColor color) const;
    [[nodiscard]] BitBoard rook_moves(Position from, PieceColor color) const;
    [[nodiscard]] BitBoard queen_moves(Position from, PieceColor color) const;
    [[nodiscard]] BitBoard king_castling_moves(PieceColor color) const;
    [[nodiscard]] BitBoard king_standard_moves(Position from, PieceColor color) const;
    [[nodiscard]] BitBoard king_moves(Position from, PieceColor color) const;
    [[nodiscard]] bool white_can_castle_kingside() const;
    [[nodiscard]] bool white_can_castle_queenside() const;
    [[nodiscard]] bool white_can_castle(BitBoard between_squares, BitBoard king_squares) const;
    [[nodiscard]] bool black_can_castle_kingside() const;
    [[nodiscard]] bool black_can_castle_queenside() const;
    [[nodiscard]] bool black_can_castle(BitBoard between_squares, BitBoard king_squares) const;
    [[nodiscard]] BitBoard black_king_castling_moves() const;
    [[nodiscard]] BitBoard white_king_castling_moves() const;

    template <Direction D>
    [[nodiscard]] BitBoard sliding_moves(BitBoard from, size_t range = BitBoard::board_size) const;
    [[nodiscard]] BitBoard sliding_moves(Direction direction, BitBoard from, size_t range = BitBoard::board_size) const;
    template <typename DirectionRange>
    [[nodiscard]] BitBoard
    sliding_moves(DirectionRange&& directions, Position from, size_t range = BitBoard::board_size) const;

    [[nodiscard]] bool is_pawn_start_square(Position from) const;
    void remove_if_color(BitBoard& moves, PieceColor color) const;
    [[nodiscard]] BitBoard board_of_color(PieceColor color) const;
    [[nodiscard]] BitBoard& board_of_color(PieceColor color);
    [[nodiscard]] BitBoard active_color_board() const;
    [[nodiscard]] BitBoard& active_color_board();
    [[nodiscard]] BitBoard inactive_color_board() const;
    [[nodiscard]] BitBoard& inactive_color_board();
};

template <Direction D>
[[nodiscard]] BitBoard Board::sliding_moves(const BitBoard from, const size_t range) const
{
    auto moves = BitBoard{from};
    for (size_t distance = 0; distance < range; ++distance) {
        moves.dilate<D>();
        if ((moves & ~from).test_any(occupied_board()) || moves.on_edge<D>()) {
            break;
        }
    }
    return moves.clear(from);
}

template <typename DirectionRange>
[[nodiscard]] BitBoard Board::sliding_moves(DirectionRange&& directions, const Position from, size_t range) const
{
    BitBoard moves;
    for (const auto direction : std::forward<DirectionRange>(directions)) {
        moves.set(sliding_moves(direction, BitBoard{from}, range));
    }
    return moves;
}

} // namespace chess
