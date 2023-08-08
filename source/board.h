#pragma once

#include "bit_board.h"
#include "pieces.h"
#include "vec2.h"

#include <optional>
#include <set>

namespace chess {

class Board
{
  private:
    struct BitBoardMove;

  public:
    using Position = BitBoard::Position;

    struct Move
    {
        Position from;
        Position to;
        constexpr static Move from_bitboard_move(const BitBoardMove move)
        {
            return {move.from.to_position(), move.to.to_position()};
        }
    };

    void clear_piece(const Position& position);
    void set_piece(Piece piece, const Position& position);
    void make_move(Move move, std::optional<PieceType> promotion_selection = std::nullopt);
    [[nodiscard]] bool occupied(const Position& position) const;
    [[nodiscard]] std::optional<PieceColor> piece_color_at(const Position& position) const;
    [[nodiscard]] std::optional<PieceType> piece_type_at(const Position& position) const;
    [[nodiscard]] std::optional<Piece> piece_at(const Position& position) const;
    [[nodiscard]] bool is_promotion_move(Move move) const;
    [[nodiscard]] std::vector<Board::Position> valid_moves_vector(Position from);
    [[nodiscard]] std::set<Board::Position> valid_moves_set(Position from);
    [[nodiscard]] PieceColor active_color() const;
    [[nodiscard]] PieceColor inactive_color() const;
    [[nodiscard]] bool is_active_piece(const Position& position) const;
    template <PieceColor Color>
    [[nodiscard]] std::vector<Board::Position> attacked_by_vector() const;
    template <PieceColor Color>
    [[nodiscard]] bool is_in_check() const;
    [[nodiscard]] bool is_active_in_check() const;
    [[nodiscard]] bool is_in_checkmate() const;
    [[nodiscard]] bool is_in_stalemate() const;
    [[nodiscard]] Board::Position active_king_position() const;

    [[nodiscard]] static Board make_standard_setup_board();

  private:
    struct BitBoardMove
    {
        BitBoard from;
        BitBoard to;
        constexpr static BitBoardMove from_move(const Move move)
        {
            return {BitBoard{move.from}, BitBoard{move.to}};
        }
    };

    struct BitBoardPieceMove
    {
        Piece piece;
        BitBoard from;
        BitBoard to;
    };

    struct BoardState
    {
        BoardState(const Board& board)
            : en_passant_square{board.en_passant_square_},
              pawns{board.pawns_},
              knights{board.knights_},
              bishops{board.bishops_},
              rooks{board.rooks_},
              queens{board.queens_},
              kings{board.kings_},
              black{board.black_},
              white{board.white_},
              active_color{board.active_color_},
              black_queenside_castle_piece_moved{board.black_queenside_castle_piece_moved_},
              black_kingside_castle_piece_moved{board.black_kingside_castle_piece_moved_},
              white_queenside_castle_piece_moved{board.white_queenside_castle_piece_moved_},
              white_kingside_castle_piece_moved{board.white_kingside_castle_piece_moved_}
        {}

        BitBoard en_passant_square;
        BitBoard pawns;
        BitBoard knights;
        BitBoard bishops;
        BitBoard rooks;
        BitBoard queens;
        BitBoard kings;
        BitBoard black;
        BitBoard white;
        PieceColor active_color{PieceColor::white};
        bool black_queenside_castle_piece_moved{false};
        bool black_kingside_castle_piece_moved{false};
        bool white_queenside_castle_piece_moved{false};
        bool white_kingside_castle_piece_moved{false};
    };

    inline static constexpr Position::dimension_type black_piece_row_index{0};
    inline static constexpr Position::dimension_type black_pawn_row_index{1};
    inline static constexpr Position::dimension_type white_piece_row_index{7};
    inline static constexpr Position::dimension_type white_pawn_row_index{6};

    template <PieceColor Color>
    [[nodiscard]] BitBoard piece_row() const;
    template <PieceColor Color>
    [[nodiscard]] BitBoard pawn_row() const;

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

    std::vector<BoardState> history_;
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
    [[nodiscard]] std::optional<PieceType> piece_type_at(BitBoard position) const;
    [[nodiscard]] std::optional<PieceColor> piece_color_at(BitBoard position) const;
    [[nodiscard]] Piece piece_at_checked(const Position& position) const;
    [[nodiscard]] Piece piece_at_checked(BitBoard position) const;
    [[nodiscard]] BitBoard occupied_board() const;
    template <PieceColor Color>
    [[nodiscard]] BitBoard attacked_by() const;
    [[nodiscard]] BitBoard attacked_by_color(PieceColor color) const;
    [[nodiscard]] bool is_color_in_check(PieceColor color) const;
    void clear_pieces(BitBoard board);
    void set_pieces(Piece piece, BitBoard positions);
    void move_piece(BitBoardPieceMove move);
    void make_move(BitBoardMove move, std::optional<PieceType> promotion_selection = std::nullopt);
    void make_move(BitBoardPieceMove move, std::optional<PieceType> promotion_selection = std::nullopt);
    void castle(BitBoardPieceMove king_move);
    void undo_previous_move();
    void white_castle(BitBoardPieceMove king_move);
    void black_castle(BitBoardPieceMove king_move);
    void update_en_passant_state(BitBoardPieceMove move);
    void update_castling_state(BitBoardPieceMove move);

    template <PieceColor Color>
    [[nodiscard]] BitBoard valid_moves_bitboard(BitBoard from) const;

    [[nodiscard]] BitBoard valid_moves_bitboard(BitBoard from) const;

    [[nodiscard]] bool is_valid_move(BitBoardMove move) const;
    [[nodiscard]] bool has_valid_move() const;
    template <PieceColor Color>
    [[nodiscard]] bool is_promotion_move(BitBoardMove move) const;
    [[nodiscard]] bool test_move_for_self_check(const BitBoardMove& move) const;
    template <PieceColor Color>
    [[nodiscard]] BitBoard attacking_bitboard(BitBoard from) const;

    template <PieceColor Color>
    [[nodiscard]] BitBoard pawn_attacking_squares(BitBoard from) const;
    template <PieceColor Color>
    [[nodiscard]] BitBoard pawn_attacking_moves(BitBoard from) const;
    template <PieceColor Color>
    [[nodiscard]] BitBoard pawn_moves(BitBoard from) const;

    [[nodiscard]] BitBoard knight_moves(BitBoard from) const;
    [[nodiscard]] BitBoard bishop_moves(BitBoard from) const;
    [[nodiscard]] BitBoard rook_moves(BitBoard from) const;
    [[nodiscard]] BitBoard queen_moves(BitBoard from) const;

    [[nodiscard]] BitBoard king_standard_moves(BitBoard from) const;
    template <PieceColor Color>
    [[nodiscard]] BitBoard king_castling_moves() const;
    template <PieceColor Color>
    [[nodiscard]] BitBoard king_moves(BitBoard from) const;
    template <PieceColor Color>
    [[nodiscard]] bool can_castle(BitBoard between_squares, BitBoard king_squares) const;
    [[nodiscard]] bool white_can_castle_kingside() const;
    [[nodiscard]] bool white_can_castle_queenside() const;
    [[nodiscard]] bool black_can_castle_kingside() const;
    [[nodiscard]] bool black_can_castle_queenside() const;

    template <Direction D>
    [[nodiscard]] BitBoard sliding_moves(BitBoard from, size_t range = BitBoard::board_size) const;
    [[nodiscard]] BitBoard sliding_moves(Direction direction, BitBoard from, size_t range = BitBoard::board_size) const;
    template <typename DirectionRange>
    [[nodiscard]] BitBoard
    sliding_moves(DirectionRange&& directions, BitBoard from, size_t range = BitBoard::board_size) const;

    template <PieceColor Color>
    [[nodiscard]] bool is_pawn_start_square(const BitBoard position) const
    {
        return pawn_row<Color>().test_any(position);
    }

    void remove_if_color(BitBoard& moves, PieceColor color) const;

    template <PieceColor Color>
    [[nodiscard]] constexpr BitBoard board_of_color() const;
    template <PieceColor Color>
    [[nodiscard]] constexpr BitBoard& board_of_color();
    [[nodiscard]] BitBoard board_of_color(PieceColor color) const;
    [[nodiscard]] BitBoard& board_of_color(PieceColor color);
    [[nodiscard]] BitBoard active_color_board() const;
    [[nodiscard]] BitBoard& active_color_board();
    [[nodiscard]] BitBoard inactive_color_board() const;
    [[nodiscard]] BitBoard& inactive_color_board();

    void set_state(const BoardState& state);
};

template <Direction D>
BitBoard Board::sliding_moves(const BitBoard from, const size_t range) const
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
BitBoard Board::sliding_moves(DirectionRange&& directions, const BitBoard from, size_t range) const
{
    BitBoard moves;
    for (const auto direction : std::forward<DirectionRange>(directions)) {
        moves.set(sliding_moves(direction, from, range));
    }
    return moves;
}

template <PieceColor Color>
BitBoard Board::pawn_attacking_squares(const BitBoard from) const
{
    assert(pawns_.test_all(from) && "not a pawn");
    if constexpr (Color == PieceColor::black) {
        return BitBoard::shift<downright>(from) | BitBoard::shift<downleft>(from);
    } else {
        return BitBoard::shift<upright>(from) | BitBoard::shift<upleft>(from);
    }
}

template <PieceColor Color>
BitBoard Board::pawn_attacking_moves(BitBoard from) const
{
    assert(pawns_.test_all(from) && "not a pawn");
    BitBoard attacking_moves = pawn_attacking_squares<Color>(from);
    if constexpr (Color == PieceColor::black) {
        return attacking_moves &= (white_ | en_passant_square_);
    } else {
        return attacking_moves &= (black_ | en_passant_square_);
    }
}

template <PieceColor Color>
BitBoard Board::pawn_moves(const BitBoard from) const
{
    assert(pawns_.test_all(from) && "not a pawn");
    const auto n_spaces = is_pawn_start_square<Color>(from) ? 2 : 1;
    BitBoard forward_moves;
    if constexpr (Color == PieceColor::white) {
        forward_moves = sliding_moves<up>(from, n_spaces).clear(occupied_board());
    } else if constexpr (Color == PieceColor::black) {
        forward_moves = sliding_moves<down>(from, n_spaces).clear(occupied_board());
    }
    return forward_moves | pawn_attacking_moves<Color>(from);
}

template <PieceColor Color>
BitBoard Board::king_castling_moves() const
{
    if constexpr (Color == PieceColor::black) {
        return (black_can_castle_kingside() ? black_castle_kingside_king_move : BitBoard{}) |
               (black_can_castle_queenside() ? black_castle_queenside_king_move : BitBoard{});
    } else {
        return (white_can_castle_kingside() ? white_castle_kingside_king_move : BitBoard{}) |
               (white_can_castle_queenside() ? white_castle_queenside_king_move : BitBoard{});
    }
}

template <PieceColor Color>
bool Board::can_castle(const BitBoard between_squares, const BitBoard king_squares) const
{
    return (
        !between_squares.test_any(occupied_board()) && !king_squares.test_any(attacked_by<opposite_color_v<Color>>())
    );
}

template <PieceColor Color>
BitBoard Board::king_moves(const BitBoard from) const
{
    assert(kings_.test_all(from) && "not a king");
    return king_standard_moves(from) | king_castling_moves<Color>();
}

template <PieceColor Color>
BitBoard Board::valid_moves_bitboard(const BitBoard from) const
{
    BitBoard moves;
    const auto piece = piece_at(from);
    if (active_color() != Color || !piece.has_value()) {
        return moves;
    }
    assert(piece->color == Color);

    switch (piece->type) {
    case PieceType::pawn:
        moves = pawn_moves<Color>(from);
        break;
    case PieceType::knight:
        moves = knight_moves(from);
        break;
    case PieceType::bishop:
        moves = bishop_moves(from);
        break;
    case PieceType::rook:
        moves = rook_moves(from);
        break;
    case PieceType::queen:
        moves = queen_moves(from);
        break;
    case PieceType::king:
        moves = king_moves<Color>(from);
        break;
    }

    for (const auto to : moves.to_bitboard_vector()) {
        if (test_move_for_self_check(BitBoardMove{from, to})) {
            moves.clear(to);
        }
    }
    moves.clear(board_of_color<Color>());

    return moves;
}

template <PieceColor Color>
[[nodiscard]] bool Board::is_promotion_move(const BitBoardMove move) const
{
    if (!move.from.test_any(pawns_)) {
        return false;
    }
    assert(piece_at_checked(move.from).color == Color);
    return piece_row<opposite_color_v<Color>>().test_all(move.to);
}

template <PieceColor Color>
BitBoard Board::attacking_bitboard(const BitBoard from) const
{
    BitBoard moves;
    const auto piece = piece_at(from);
    if (active_color() != Color || !piece.has_value()) {
        return moves;
    }
    assert(piece->color == Color);

    switch (piece->type) {
    case PieceType::pawn:
        moves = pawn_attacking_squares<Color>(from);
        break;
    case PieceType::knight:
        moves = knight_moves(from);
        break;
    case PieceType::bishop:
        moves = bishop_moves(from);
        break;
    case PieceType::rook:
        moves = rook_moves(from);
        break;
    case PieceType::queen:
        moves = queen_moves(from);
        break;
    case PieceType::king:
        moves = king_standard_moves(from);
        break;
    }

    moves.clear(board_of_color<Color>());

    return moves;
}

template <PieceColor Color>
BitBoard Board::attacked_by() const
{
    BitBoard attacked_by;
    for (const auto position : board_of_color<Color>().to_bitboard_vector()) {
        attacked_by.set(attacking_bitboard<Color>(position));
    }
    return attacked_by;
}

template <PieceColor Color>
std::vector<Board::Position> Board::attacked_by_vector() const
{
    return attacked_by<Color>().to_position_vector();
}

template <PieceColor Color>
bool Board::is_in_check() const
{
    const auto king = board_of_color<Color>() & kings_;
    return king.test_any(attacked_by<opposite_color_v<Color>>());
}

template <PieceColor Color>
constexpr BitBoard Board::board_of_color() const
{
    if constexpr (Color == PieceColor::black) {
        return black_;
    } else {
        return white_;
    }
}

template <PieceColor Color>
constexpr BitBoard& Board::board_of_color()
{
    if constexpr (Color == PieceColor::black) {
        return black_;
    } else {
        return white_;
    }
}

inline BitBoard Board::board_of_color(PieceColor color) const
{
    return color == PieceColor::black ? black_ : white_;
}

inline BitBoard& Board::board_of_color(PieceColor color)
{
    return color == PieceColor::black ? black_ : white_;
}

inline BitBoard& Board::active_color_board()
{
    return board_of_color(active_color_);
}

inline BitBoard Board::active_color_board() const
{
    return board_of_color(active_color_);
}

inline BitBoard& Board::inactive_color_board()
{
    return board_of_color(opposite_color(active_color_));
}

inline BitBoard Board::inactive_color_board() const
{
    return board_of_color(opposite_color(active_color_));
}

template <PieceColor Color>
BitBoard Board::piece_row() const
{
    if constexpr (Color == PieceColor::black) {
        return BitBoard::make_row(black_piece_row_index);
    } else {
        return BitBoard::make_row(white_piece_row_index);
    }
}

template <PieceColor Color>
BitBoard Board::pawn_row() const
{
    if constexpr (Color == PieceColor::black) {
        return BitBoard::make_row(black_pawn_row_index);
    } else {
        return BitBoard::make_row(white_pawn_row_index);
    }
}

} // namespace chess
