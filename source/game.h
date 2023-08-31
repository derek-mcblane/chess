#pragma once

#include "bit_board.h"
#include "board.h"
#include "pieces.h"
#include "vec2.h"

#include <optional>
#include <set>
#include <utility>

namespace chess {

class GameBoard
{
  public:
    using Position = BoardPieces::Position;
    using Move = BoardPieces::Move;

    [[nodiscard]] std::optional<Piece> piece_at(Position position) const;
    void make_move(Move move, std::optional<PieceType> promotion_selection = std::nullopt);
    [[nodiscard]] bool is_promotion_move(Move move) const;
    [[nodiscard]] std::vector<Position> valid_moves_vector(Position from);
    [[nodiscard]] std::set<Position> valid_moves_set(Position from);
    [[nodiscard]] PieceColor active_color() const;
    [[nodiscard]] PieceColor inactive_color() const;
    [[nodiscard]] bool is_active_piece(const Position& position) const;
    template <PieceColor Color>
    [[nodiscard]] std::vector<Position> attacked_by_vector() const;
    template <PieceColor Color>
    [[nodiscard]] bool is_in_check() const;
    [[nodiscard]] bool is_active_in_check() const;
    [[nodiscard]] bool is_in_checkmate() const;
    [[nodiscard]] bool is_in_stalemate() const;
    [[nodiscard]] Position active_king_position() const;

  private:
    using BitBoardMove = BoardPieces::BitBoardMove;

    enum class CastlingSide
    {
        queenside,
        kingside
    };

    struct BoardState
    {
        BoardState(const GameBoard& board)
            : pieces{board.pieces_},
              en_passant_square{board.en_passant_square_},
              active_color{board.active_color_},
              black_queenside_castle_piece_moved{board.black_queenside_castle_piece_moved_},
              black_kingside_castle_piece_moved{board.black_kingside_castle_piece_moved_},
              white_queenside_castle_piece_moved{board.white_queenside_castle_piece_moved_},
              white_kingside_castle_piece_moved{board.white_kingside_castle_piece_moved_}
        {}

        BoardPieces pieces;
        BitBoard en_passant_square;
        PieceColor active_color{PieceColor::white};
        bool black_queenside_castle_piece_moved{false};
        bool black_kingside_castle_piece_moved{false};
        bool white_queenside_castle_piece_moved{false};
        bool white_kingside_castle_piece_moved{false};
    };

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
    BoardPieces pieces_{BoardPieces::make_standard_setup_board()};
    BitBoard en_passant_square_;
    PieceColor active_color_{PieceColor::white};
    bool black_queenside_castle_piece_moved_{false};
    bool black_kingside_castle_piece_moved_{false};
    bool white_queenside_castle_piece_moved_{false};
    bool white_kingside_castle_piece_moved_{false};

    [[nodiscard]] BitBoard pawns() const
    {
        return pieces_.pawns();
    }
    [[nodiscard]] BitBoard knights() const
    {
        return pieces_.knights();
    }
    [[nodiscard]] BitBoard bishops() const
    {
        return pieces_.bishops();
    }
    [[nodiscard]] BitBoard rooks() const
    {
        return pieces_.rooks();
    }
    [[nodiscard]] BitBoard queens() const
    {
        return pieces_.queens();
    }
    [[nodiscard]] BitBoard kings() const
    {
        return pieces_.kings();
    }

    [[nodiscard]] BitBoard white() const
    {
        return pieces_.white();
    }
    [[nodiscard]] BitBoard black() const
    {
        return pieces_.black();
    }

    [[nodiscard]] BitBoard occupied() const
    {
        return pieces_.occupied();
    }

    template <PieceColor Color>
    [[nodiscard]] BitBoard attacked_by() const;
    [[nodiscard]] BitBoard attacked_by_color(PieceColor color) const;
    [[nodiscard]] bool is_color_in_check(PieceColor color) const;
    void make_move(BitBoardMove move, std::optional<PieceType> promotion_selection = std::nullopt);
    void make_move(Piece piece, BitBoardMove move, std::optional<PieceType> promotion_selection = std::nullopt);
    void castle(Piece piece, BitBoardMove king_move);
    void undo_previous_move();
    void white_castle(BitBoardMove king_move);
    void black_castle(BitBoardMove king_move);
    void update_en_passant_state(Piece piece, BitBoardMove move);
    void update_castling_state(BitBoardMove move);

    template <PieceColor Color>
    [[nodiscard]] BitBoard valid_moves_bitboard(BitBoard from) const;
    [[nodiscard]] BitBoard valid_moves_bitboard(BitBoard from) const;
    [[nodiscard]] bool is_valid_move(BitBoardMove move) const;
    [[nodiscard]] bool has_valid_move() const;
    template <PieceColor Color>
    [[nodiscard]] bool is_promotion_move(BitBoardMove move) const;
    template <PieceColor Color>
    [[nodiscard]] bool is_castling_move(BitBoardMove move) const;
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

    template <PieceColor Color>
    [[nodiscard]] bool is_pawn_start_square(const BitBoard position) const
    {
        return pawn_row<Color>().test_any(position);
    }

    [[nodiscard]] BitBoard active_color_board() const;
    [[nodiscard]] BitBoard inactive_color_board() const;

    void set_state(const BoardState& state);
};

template <PieceColor Color>
BitBoard GameBoard::pawn_attacking_squares(const BitBoard from) const
{
    assert(pawns().test_all(from) && "not a pawn");
    if constexpr (Color == PieceColor::black) {
        return BitBoard::shift<downright>(from) | BitBoard::shift<downleft>(from);
    } else {
        return BitBoard::shift<upright>(from) | BitBoard::shift<upleft>(from);
    }
}

template <PieceColor Color>
BitBoard GameBoard::pawn_attacking_moves(BitBoard from) const
{
    assert(pawns().test_all(from) && "not a pawn");
    BitBoard attacking_moves = pawn_attacking_squares<Color>(from);
    if constexpr (Color == PieceColor::black) {
        return attacking_moves &= (white() | en_passant_square_);
    } else {
        return attacking_moves &= (black() | en_passant_square_);
    }
}

template <PieceColor Color>
BitBoard GameBoard::pawn_moves(const BitBoard from) const
{
    assert(pawns().test_all(from) && "not a pawn");
    const auto n_spaces = is_pawn_start_square<Color>(from) ? 2 : 1;
    BitBoard forward_moves;
    if constexpr (Color == PieceColor::white) {
        forward_moves = pieces_.sliding_moves<up>(from, n_spaces).clear(occupied());
    } else if constexpr (Color == PieceColor::black) {
        forward_moves = pieces_.sliding_moves<down>(from, n_spaces).clear(occupied());
    }
    return forward_moves | pawn_attacking_moves<Color>(from);
}

template <PieceColor Color>
BitBoard GameBoard::king_castling_moves() const
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
bool GameBoard::can_castle(const BitBoard between_squares, const BitBoard king_squares) const
{
    return (!between_squares.test_any(occupied()) && !king_squares.test_any(attacked_by<opposite_color_v<Color>>()));
}

template <PieceColor Color>
BitBoard GameBoard::king_moves(const BitBoard from) const
{
    assert(kings().test_all(from) && "not a king");
    return king_standard_moves(from) | king_castling_moves<Color>();
}

template <PieceColor Color>
BitBoard GameBoard::valid_moves_bitboard(const BitBoard from) const
{
    BitBoard moves;
    const auto piece = pieces_.at(from);
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
    moves.clear(pieces_.of<Color>());

    return moves;
}

template <PieceColor Color>
[[nodiscard]] bool GameBoard::is_promotion_move(const BitBoardMove move) const
{
    if (!move.from.test_any(pawns())) {
        return false;
    }
    assert(pieces_.at_checked(move.from).color == Color);
    return piece_row<opposite_color_v<Color>>().test_all(move.to);
}

// TODO: remove attacks that would self-check
template <PieceColor Color>
BitBoard GameBoard::attacking_bitboard(const BitBoard from) const
{
    BitBoard moves;
    const auto piece = pieces_.at(from);
    if (!piece.has_value()) {
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

    moves.clear(pieces_.of<Color>());

    return moves;
}

template <PieceColor Color>
BitBoard GameBoard::attacked_by() const
{
    BitBoard attacked_by;
    for (const auto position : pieces_.of<Color>().to_bitboard_vector()) {
        attacked_by.set(attacking_bitboard<Color>(position));
    }
    return attacked_by;
}

template <PieceColor Color>
std::vector<GameBoard::Position> GameBoard::attacked_by_vector() const
{
    return attacked_by<Color>().to_position_vector();
}

template <PieceColor Color>
bool GameBoard::is_in_check() const
{
    const auto king = pieces_.of<Piece{Color, PieceType::king}>();
    return king.test_any(attacked_by<opposite_color_v<Color>>());
}

inline BitBoard GameBoard::active_color_board() const
{
    return pieces_.of(active_color_);
}
inline BitBoard GameBoard::inactive_color_board() const
{
    return pieces_.of(opposite_color(active_color_));
}

} // namespace chess
