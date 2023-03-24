#include "pieces.h"

#include "bit_board_formatter.h"

#include <spdlog/spdlog.h>

#include <cassert>

namespace chess {

const std::map<PieceColor, std::string> piece_color_names{
    {PieceColor::white, "white"},
    {PieceColor::black, "black"},
    {PieceColor::none, "none"},
};

const std::map<PieceType, std::string> piece_type_names{
    {PieceType::pawn, "pawn"},
    {PieceType::knight, "knight"},
    {PieceType::bishop, "bishop"},
    {PieceType::rook, "rook"},
    {PieceType::queen, "queen"},
    {PieceType::king, "king"},
    {PieceType::none, "none"},
};

[[nodiscard]] PieceType next_piece_type(const PieceType type)
{
    switch (type) {
    case PieceType::pawn:
        return PieceType::knight;
    case PieceType::knight:
        return PieceType::bishop;
    case PieceType::bishop:
        return PieceType::rook;
    case PieceType::rook:
        return PieceType::queen;
    case PieceType::queen:
        return PieceType::king;
    case PieceType::king:
        return PieceType::none;
    case PieceType::none:
        return PieceType::pawn;
    default:
        assert(!"invalid PieceType");
        return PieceType::none;
    }
}

[[nodiscard]] PieceColor next_piece_color(const PieceColor color)
{
    switch (color) {
    case PieceColor::black:
        return PieceColor::white;
    case PieceColor::white:
        return PieceColor::none;
    case PieceColor::none:
        return PieceColor::black;
    default:
        assert(!"invalid PieceColor");
        return PieceColor::none;
    }
}

[[nodiscard]] PieceColor toggle_piece_color(const PieceColor color)
{
    switch (color) {
    case PieceColor::black:
        return PieceColor::white;
    case PieceColor::white:
        return PieceColor::black;
    case PieceColor::none:
    default:
        assert(!"invalid PieceColor");
        return PieceColor::none;
    }
}

void BoardPieces::set_piece(const Piece& piece, const Coord& position)
{
    clear_piece(position);

    if (piece == null_piece) {
        return;
    }

    occupied_.set(position);
    switch (piece.type) {
    case PieceType::pawn:
        pawns_.set(position);
        break;
    case PieceType::knight:
        knights_.set(position);
        break;
    case PieceType::bishop:
        bishops_.set(position);
        break;
    case PieceType::rook:
        rooks_.set(position);
        break;
    case PieceType::queen:
        queens_.set(position);
        break;
    case PieceType::king:
        kings_.set(position);
        break;
    case PieceType::none:
    default:
        assert(!"no piece type");
        break;
    }

    switch (piece.color) {
    case PieceColor::black:
        black_.set(position);
        break;
    case PieceColor::white:
        white_.set(position);
        break;
    case PieceColor::none:
    default:
        assert(!"no color type");
        break;
    }
}

void BoardPieces::move_piece(const Coord& from, const Coord& to)
{
    assert(occupied(from) && "move from empty square");

    if (from == to) {
        return;
    }

    set_piece(occupant_at(from), to);
    clear_piece(from);
}

void BoardPieces::clear_piece(const Coord& position)
{
    if (!occupied(position)) {
        return;
    }

    const auto occupant = occupant_at(position);

    switch (occupant.type) {
    case PieceType::pawn:
        pawns_.reset(position);
        break;
    case PieceType::knight:
        knights_.reset(position);
        break;
    case PieceType::bishop:
        bishops_.reset(position);
        break;
    case PieceType::rook:
        rooks_.reset(position);
        break;
    case PieceType::queen:
        queens_.reset(position);
        break;
    case PieceType::king:
        kings_.reset(position);
        break;
    case PieceType::none:
        assert(!"occupied by PieceType::none");
        break;
    }

    switch (occupant.color) {
    case PieceColor::black:
        black_.reset(position);
        break;
    case PieceColor::white:
        white_.reset(position);
        break;
    case PieceColor::none:
        assert(!"occupied by PieceColor::none");
        break;
    }

    occupied_.reset(position);
}

[[nodiscard]] bool BoardPieces::occupied(const Coord& position) const
{
    return occupied_.test(position);
}

[[nodiscard]] Piece BoardPieces::occupant_at(const Coord& position) const
{
    if (!occupied(position)) {
        return null_piece;
    }
    return Piece{.color = piece_color_at(position), .type = piece_type_at(position)};
}

[[nodiscard]] bool BoardPieces::is_valid_move(const Coord& from, const Coord& to)
{
    return valid_moves_bitboard(from).test(to);
}

[[nodiscard]] BitBoard BoardPieces::valid_moves(const Coord& from)
{
    return valid_moves_bitboard(from);
}

[[nodiscard]] PieceColor BoardPieces::piece_color_at(const Coord& position) const
{
    if (!occupied(position)) {
        return PieceColor::none;
    }
    if (white_.test(position)) {
        return PieceColor::white;
    }
    if (black_.test(position)) {
        return PieceColor::black;
    }
    return PieceColor::none;
}

[[nodiscard]] PieceType BoardPieces::piece_type_at(const Coord& position) const
{
    if (!occupied(position)) {
        return PieceType::none;
    }
    if (pawns_.test(position)) {
        return PieceType::pawn;
    }
    if (knights_.test(position)) {
        return PieceType::knight;
    }
    if (bishops_.test(position)) {
        return PieceType::bishop;
    }
    if (rooks_.test(position)) {
        return PieceType::rook;
    }
    if (queens_.test(position)) {
        return PieceType::queen;
    }
    if (kings_.test(position)) {
        return PieceType::king;
    }
    return PieceType::none;
}

[[nodiscard]] BitBoard BoardPieces::pawn_moves(const Coord& from)
{
    if (piece_color_at(from) == PieceColor::white) {
        return BitBoard{BitBoard::position_mask(from)}.dilate<up>();
    } else {
        return BitBoard{BitBoard::position_mask(from)}.dilate<down>();
    }
}

[[nodiscard]] BitBoard BoardPieces::knight_moves(const Coord& from)
{
    return ~0ULL;
}

[[nodiscard]] BitBoard BoardPieces::bishop_moves(const Coord& from)
{
    return ~0ULL;
}

[[nodiscard]] BitBoard BoardPieces::rook_moves(const Coord& from)
{
    return ~0ULL;
}

[[nodiscard]] BitBoard BoardPieces::queen_moves(const Coord& from)
{
    return ~0ULL;
}

[[nodiscard]] BitBoard BoardPieces::king_moves(const Coord& from)
{
    const auto moves = BitBoard::neighbors_cardinal_and_diagonal(from);
    spdlog::debug("king moves: {}", moves);
    return moves;
}

[[nodiscard]] BitBoard BoardPieces::valid_moves_bitboard(const Coord& from)
{
    BitBoard moves;
    switch (piece_type_at(from)) {
    case PieceType::pawn:
        moves = pawn_moves(from);
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
        moves = king_moves(from);
        break;
    case PieceType::none:
    default:
        break;
    }
    return moves;
}

/// @todo: use loops
[[nodiscard]] BoardPieces BoardPieces::make_standard_setup_board()
{
    BoardPieces pieces;

    pieces.set_piece({PieceColor::black, PieceType::rook}, {0, 0});
    pieces.set_piece({PieceColor::black, PieceType::bishop}, {0, 1});
    pieces.set_piece({PieceColor::black, PieceType::knight}, {0, 2});
    pieces.set_piece({PieceColor::black, PieceType::queen}, {0, 3});
    pieces.set_piece({PieceColor::black, PieceType::king}, {0, 4});
    pieces.set_piece({PieceColor::black, PieceType::bishop}, {0, 5});
    pieces.set_piece({PieceColor::black, PieceType::knight}, {0, 6});
    pieces.set_piece({PieceColor::black, PieceType::rook}, {0, 7});

    pieces.set_piece({PieceColor::black, PieceType::pawn}, {1, 0});
    pieces.set_piece({PieceColor::black, PieceType::pawn}, {1, 1});
    pieces.set_piece({PieceColor::black, PieceType::pawn}, {1, 2});
    pieces.set_piece({PieceColor::black, PieceType::pawn}, {1, 3});
    pieces.set_piece({PieceColor::black, PieceType::pawn}, {1, 4});
    pieces.set_piece({PieceColor::black, PieceType::pawn}, {1, 5});
    pieces.set_piece({PieceColor::black, PieceType::pawn}, {1, 6});
    pieces.set_piece({PieceColor::black, PieceType::pawn}, {1, 7});

    pieces.set_piece({PieceColor::white, PieceType::pawn}, {6, 0});
    pieces.set_piece({PieceColor::white, PieceType::pawn}, {6, 1});
    pieces.set_piece({PieceColor::white, PieceType::pawn}, {6, 2});
    pieces.set_piece({PieceColor::white, PieceType::pawn}, {6, 3});
    pieces.set_piece({PieceColor::white, PieceType::pawn}, {6, 4});
    pieces.set_piece({PieceColor::white, PieceType::pawn}, {6, 5});
    pieces.set_piece({PieceColor::white, PieceType::pawn}, {6, 6});
    pieces.set_piece({PieceColor::white, PieceType::pawn}, {6, 7});

    pieces.set_piece({PieceColor::white, PieceType::rook}, {7, 0});
    pieces.set_piece({PieceColor::white, PieceType::bishop}, {7, 1});
    pieces.set_piece({PieceColor::white, PieceType::knight}, {7, 2});
    pieces.set_piece({PieceColor::white, PieceType::queen}, {7, 3});
    pieces.set_piece({PieceColor::white, PieceType::king}, {7, 4});
    pieces.set_piece({PieceColor::white, PieceType::bishop}, {7, 5});
    pieces.set_piece({PieceColor::white, PieceType::knight}, {7, 6});
    pieces.set_piece({PieceColor::white, PieceType::rook}, {7, 7});

    return pieces;
}

} // namespace chess
