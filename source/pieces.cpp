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

void BoardPieces::clear_pieces(const BitBoard board)
{
    occupied_.reset(board);
    black_.reset(board);
    white_.reset(board);
    pawns_.reset(board);
    knights_.reset(board);
    bishops_.reset(board);
    rooks_.reset(board);
    queens_.reset(board);
    kings_.reset(board);
}

void BoardPieces::set_pieces(const Piece& piece, const BitBoard positions)
{
    clear_pieces(positions);

    if (piece == null_piece) {
        return;
    }

    occupied_.set(positions);
    switch (piece.type) {
    case PieceType::pawn:
        pawns_.set(positions);
        break;
    case PieceType::knight:
        knights_.set(positions);
        break;
    case PieceType::bishop:
        bishops_.set(positions);
        break;
    case PieceType::rook:
        rooks_.set(positions);
        break;
    case PieceType::queen:
        queens_.set(positions);
        break;
    case PieceType::king:
        kings_.set(positions);
        break;
    case PieceType::none:
        assert(!"no piece type");
        break;
    }

    switch (piece.color) {
    case PieceColor::black:
        black_.set(positions);
        break;
    case PieceColor::white:
        white_.set(positions);
        break;
    case PieceColor::none:
        assert(!"no color type");
        break;
    }
}

void BoardPieces::clear_piece(const Position& position)
{
    clear_squares_attacked_by(position);
    clear_pieces(BitBoard::make_from_position(position));
}

void BoardPieces::set_piece(const Piece& piece, const Position& position)
{
    set_pieces(piece, BitBoard::make_from_position(position));
    set_squares_attacked_by(position);
}

void BoardPieces::update_after_move(const Move move)
{
    update_en_passant_state(move);
    update_castling_state(move);
    moves_.push_back(move);
}

void BoardPieces::update_en_passant_state(const Move move)
{
    en_passant_square_.reset_all();
    if (move.piece.type == PieceType::pawn) {
        if (Position::chebyshev_distance(move.from, move.to) == 2) {
            en_passant_square_.set((move.from + move.to) / 2);
        }
    }
}

void BoardPieces::update_castling_state(const Move move)
{
    if (move.from == BoardPieces::black_queenside_rook_position) {
        black_queenside_castle_piece_moved_ = true;
    }
    if (move.from == BoardPieces::black_kingside_rook_position) {
        black_kingside_castle_piece_moved_ = true;
    }
    if (move.from == BoardPieces::black_king_position) {
        black_queenside_castle_piece_moved_ = true;
        black_kingside_castle_piece_moved_ = true;
    }
    if (move.from == BoardPieces::white_queenside_rook_position) {
        white_queenside_castle_piece_moved_ = true;
    }
    if (move.from == BoardPieces::white_kingside_rook_position) {
        white_kingside_castle_piece_moved_ = true;
    }
    if (move.from == BoardPieces::white_king_position) {
        white_queenside_castle_piece_moved_ = true;
        white_kingside_castle_piece_moved_ = true;
    }
}

void BoardPieces::move_piece(const Position& from, const Position& to)
{
    assert(occupied(from) && "move from empty square");
    assert(from != to && "move `from == to`");

    set_piece(piece_at(from), to);
    clear_piece(from);

    update_after_move(Move{piece_at(to), from, to});
}

void BoardPieces::clear_squares_attacked_by(const Position& position)
{
    assert(occupied(position) && "square not occupied");
    for (const auto attacked_square : valid_moves_bitboard(position).to_position_vector()) {
        attacked_by_[attacked_square].reset(position);
    }
}

void BoardPieces::set_squares_attacked_by(const Position& position)
{
    assert(occupied(position) && "square not occupied");
    for (const auto attacked_square : valid_moves_bitboard(position).to_position_vector()) {
        attacked_by_[attacked_square].set(position);
    }
}

void BoardPieces::clear_all()
{
    clear_pieces(BitBoard::make_full());
}

[[nodiscard]] bool BoardPieces::occupied(const Position& position) const
{
    return occupied(BitBoard::make_from_position(position));
}

[[nodiscard]] bool BoardPieces::occupied(const BitBoard position) const
{
    return occupied_.test_all(position);
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

[[nodiscard]] Piece BoardPieces::piece_at(const Position& position) const
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

bool BoardPieces::on_black_pawn_start_square(const Position& from)
{
    return from.x() == black_pawn_row;
}

bool BoardPieces::on_white_pawn_start_square(const Position& from)
{
    return from.x() == white_pawn_row;
}

bool BoardPieces::on_pawn_start_square(const Position& from) const
{
    const auto color = piece_color_at(from);
    if (color == PieceColor::black) {
        return on_black_pawn_start_square(from);
    }
    if (color == PieceColor::white) {
        return on_white_pawn_start_square(from);
    }
    assert(!"no color type");
    return false;
}

[[nodiscard]] BitBoard BoardPieces::pawn_moves(const Position& from) const
{
    const auto color = piece_color_at(from);
    const auto from_board = BitBoard::make_from_position(from);
    BitBoard moves;
    BitBoard attacking_moves;
    auto n_spaces = on_pawn_start_square(from) ? 2 : 1;
    if (color == PieceColor::white) {
        moves = sliding_moves<up>(from, n_spaces);
        attacking_moves.set(BitBoard::shift<upright>(from_board)).set(BitBoard::shift<upleft>(from_board));
        attacking_moves.reset(white_);
    }
    if (color == PieceColor::black) {
        moves = sliding_moves<down>(from, n_spaces);
        attacking_moves.set(BitBoard::shift<downright>(from_board)).set(BitBoard::shift<downleft>(from_board));
        attacking_moves.reset(black_);
    }
    moves.reset(occupied_);
    const auto color_board = color == PieceColor::black ? black_ : white_;
    attacking_moves.reset(~occupied_ & ~en_passant_square_);
    return moves | attacking_moves;
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
    auto moves = BitBoard{knight_moves_board}.shift_assign(offset);
    remove_if_color(moves, piece_color_at(from));
    moves.reset(from);
    return moves;
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
    auto moves = BitBoard::neighbors_cardinal_and_diagonal(from);
    remove_if_color(moves, piece_color_at(from));
    moves.reset(from);
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

BitBoard BoardPieces::sliding_moves(const Position& from, const Direction direction, const size_t range) const
{
    switch (direction) {
    case right:
        return sliding_moves<right>(from, range);
    case upright:
        return sliding_moves<upright>(from, range);
    case up:
        return sliding_moves<up>(from, range);
    case upleft:
        return sliding_moves<upleft>(from, range);
    case left:
        return sliding_moves<left>(from, range);
    case downleft:
        return sliding_moves<downleft>(from, range);
    case down:
        return sliding_moves<down>(from, range);
    case downright:
        return sliding_moves<downright>(from, range);
        break;
    }
}

void BoardPieces::remove_if_color(BitBoard& moves, const PieceColor& color) const
{
    if (color == PieceColor::white) {
        moves.reset(white_);
    }
    if (color == PieceColor::black) {
        moves.reset(black_);
    }
}

BoardPieces BoardPieces::make_standard_setup_board()
{
    static constexpr std::array<PieceType, 8> back_row{
        PieceType::rook,
        PieceType::knight,
        PieceType::bishop,
        PieceType::queen,
        PieceType::king,
        PieceType::bishop,
        PieceType::knight,
        PieceType::rook};

    BoardPieces pieces;
    for (Position::dimension_type y = 0; y < BitBoard::board_size; ++y) {
        pieces.set_piece({PieceColor::black, back_row[y]}, {black_piece_row, y});
        pieces.set_piece({PieceColor::white, back_row[y]}, {white_piece_row, y});
    }
    for (Position::dimension_type y = 0; y < BitBoard::board_size; ++y) {
        pieces.set_piece({PieceColor::black, PieceType::pawn}, {black_pawn_row, y});
        pieces.set_piece({PieceColor::white, PieceType::pawn}, {white_pawn_row, y});
    }
    return pieces;
}

} // namespace chess
