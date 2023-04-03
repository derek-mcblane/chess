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

void BoardPieces::set_piece(const Piece& piece, const Position& position)
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

void BoardPieces::move_piece(const Position& from, const Position& to)
{
    assert(occupied(from) && "move from empty square");

    if (from == to) {
        return;
    }

    set_piece(occupant_at(from), to);
    clear_piece(from);
}

void BoardPieces::clear_piece(const Position& position)
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

[[nodiscard]] bool BoardPieces::occupied(const Position& position) const
{
    return occupied(BitBoard::make_from_position(position));
}

[[nodiscard]] bool BoardPieces::occupied(const BitBoard position) const
{
    return occupied_.test_all(position);
}

[[nodiscard]] Piece BoardPieces::occupant_at(const Position& position) const
{
    if (!occupied(position)) {
        return null_piece;
    }
    return Piece{.color = piece_color_at(position), .type = piece_type_at(position)};
}

[[nodiscard]] bool BoardPieces::is_valid_move(const Position& from, const Position& to)
{
    return valid_moves_bitboard(from).test(to);
}

[[nodiscard]] BitBoard BoardPieces::valid_moves(const Position& from)
{
    return valid_moves_bitboard(from);
}

[[nodiscard]] PieceColor BoardPieces::piece_color_at(const Position& position) const
{
    return piece_color_at(BitBoard::make_from_position(position));
}

[[nodiscard]] PieceColor BoardPieces::piece_color_at(const BitBoard& position) const
{
    if (!occupied(position)) {
        return PieceColor::none;
    }
    if (black_.test_all(position)) {
        return PieceColor::black;
    }
    if (white_.test_all(position)) {
        return PieceColor::white;
    }
    return PieceColor::none;
}

[[nodiscard]] PieceType BoardPieces::piece_type_at(const Position& position) const
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

[[nodiscard]] BitBoard BoardPieces::pawn_moves(const Position& from) const
{
    switch (piece_color_at(from)) {
    case PieceColor::white:
        return BitBoard::make_from_position(from).dilate<up>();
    case PieceColor::black:
        return BitBoard::make_from_position(from).dilate<down>();
    case PieceColor::none:
        assert(!"no pawn at `from` position");
    }
    return {};
}

[[nodiscard]] BitBoard BoardPieces::knight_moves(const Position& from) const
{
    static const BitBoard knight_moves_board{std::bitset<64>{"01010000"
                                                             "10001000"
                                                             "00000000"
                                                             "10001000"
                                                             "01010000"
                                                             "00000000"
                                                             "00000000"
                                                             "00000000"}};
    static const Position knight_moves_origin{2, 2};

    const Position offset = from - knight_moves_origin;
    return BitBoard{knight_moves_board}.shift_assign(offset);
}

[[nodiscard]] BitBoard BoardPieces::bishop_moves(const Position& from) const
{
    static constexpr std::array<Direction, 4> directions = {
        Direction::upright, Direction::upleft, Direction::downleft, Direction::downright};
    return sliding_moves(from, directions);
}

[[nodiscard]] BitBoard BoardPieces::rook_moves(const Position& from) const
{
    static constexpr std::array<Direction, 4> directions = {
        Direction::right, Direction::up, Direction::left, Direction::down};
    return sliding_moves(from, directions);
}

[[nodiscard]] BitBoard BoardPieces::queen_moves(const Position& from) const
{
    static constexpr std::array<Direction, 8> directions = {
        Direction::right,
        Direction::upright,
        Direction::up,
        Direction::upleft,
        Direction::left,
        Direction::downleft,
        Direction::down,
        Direction::downright};
    return sliding_moves(from, directions);
}

[[nodiscard]] BitBoard BoardPieces::king_moves(const Position& from) const
{
    const auto moves = BitBoard::neighbors_cardinal_and_diagonal(from);
    spdlog::debug("king moves: {}", moves);
    return moves;
}

[[nodiscard]] BitBoard BoardPieces::valid_moves_bitboard(const Position& from) const
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

BoardPieces BoardPieces::make_standard_setup_board()
{
    static const std::vector<PieceType> back_row{
        PieceType::rook,
        PieceType::bishop,
        PieceType::knight,
        PieceType::queen,
        PieceType::king,
        PieceType::bishop,
        PieceType::knight,
        PieceType::rook};

    BoardPieces pieces;
    for (Position::dimension_type y = 0; y < BitBoard::board_size; ++y) {
        pieces.set_piece({PieceColor::black, back_row[y]}, {0, y});
        pieces.set_piece({PieceColor::white, back_row[y]}, {7, y});
    }
    for (Position::dimension_type y = 0; y < BitBoard::board_size; ++y) {
        pieces.set_piece({PieceColor::black, PieceType::pawn}, {1, y});
        pieces.set_piece({PieceColor::white, PieceType::pawn}, {6, y});
    }
    return pieces;
}

} // namespace chess
