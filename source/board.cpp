#include "board.h"

#include "vec2.h"

#include "piece_formatter.h"
#include "vec2_formatter.h"

#include <spdlog/spdlog.h>

#include <cassert>
#include <map>
#include <optional>
#include <set>

namespace chess {

void Board::clear_pieces(const BitBoard board)
{
    black_.clear(board);
    white_.clear(board);
    pawns_.clear(board);
    knights_.clear(board);
    bishops_.clear(board);
    rooks_.clear(board);
    queens_.clear(board);
    kings_.clear(board);
}

void Board::clear_piece(const Position& position)
{
    clear_pieces(BitBoard{position});
}

void Board::set_pieces(const Piece piece, const BitBoard positions)
{
    clear_pieces(positions);
    assert(!occupied_board().test_any(positions));

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
    }

    switch (piece.color) {
    case PieceColor::black:
        black_.set(positions);
        break;
    case PieceColor::white:
        white_.set(positions);
        break;
    }
}

void Board::set_piece(const Piece piece, const Position& position)
{
    set_pieces(piece, BitBoard{position});
}

void Board::update_en_passant_state(const BitBoardPieceMove move)
{
    en_passant_square_.clear_all();
    if (move.piece.type == PieceType::pawn) {
        const auto from_position = move.from.to_position();
        const auto to_position = move.to.to_position();
        if (Position::chebyshev_distance(from_position, to_position) == 2) {
            en_passant_square_.set((from_position + to_position) / 2);
        }
    }
}

void Board::update_castling_state(const BitBoardPieceMove move)
{
    if (move.from == Board::black_queenside_rook_position) {
        black_queenside_castle_piece_moved_ = true;
    }
    if (move.from == Board::black_kingside_rook_position) {
        black_kingside_castle_piece_moved_ = true;
    }
    if (move.from == Board::black_king_position) {
        black_queenside_castle_piece_moved_ = true;
        black_kingside_castle_piece_moved_ = true;
    }
    if (move.from == Board::white_queenside_rook_position) {
        white_queenside_castle_piece_moved_ = true;
    }
    if (move.from == Board::white_kingside_rook_position) {
        white_kingside_castle_piece_moved_ = true;
    }
    if (move.from == Board::white_king_position) {
        white_queenside_castle_piece_moved_ = true;
        white_kingside_castle_piece_moved_ = true;
    }
}

void Board::move_piece(const BitBoardPieceMove move)
{
    assert(piece_at_checked(move.from) == move.piece);
    spdlog::debug("moving {} from {} to {}", move.piece, move.from.to_position(), move.to.to_position());
    set_pieces(move.piece, move.to);
    clear_pieces(move.from);
}

void Board::promote(PromotionMove move)
{

}

void Board::promote_piece(const BitBoardMove move)
{

}

void Board::white_castle(const BitBoardPieceMove king_move)
{
    const auto white_moves = white_king_castling_moves();
    if ((white_moves & white_castle_kingside_king_move) == king_move.to) {
        move_piece({pieces::white_rook, white_kingside_rook_position, white_castle_kingside_rook_move});
    }
    if ((white_moves & white_castle_queenside_king_move) == king_move.to) {
        move_piece({pieces::white_rook, white_queenside_rook_position, white_castle_queenside_rook_move});
    }
}

void Board::black_castle(const BitBoardPieceMove king_move)
{
    const auto black_moves = black_king_castling_moves();
    if ((black_moves & black_castle_kingside_king_move) == king_move.to) {
        move_piece({pieces::black_rook, black_kingside_rook_position, black_castle_kingside_rook_move});
    }
    if ((black_moves & black_castle_queenside_king_move) == king_move.to) {
        move_piece({pieces::black_rook, black_queenside_rook_position, black_castle_queenside_rook_move});
    }
}

void Board::castle(const BitBoardPieceMove king_move)
{
    switch (king_move.piece.color) {
    case PieceColor::black:
        black_castle(king_move);
    case PieceColor::white:
        white_castle(king_move);
        break;
    }
}

void Board::make_move(Move move)
{
    assert(move.from != move.to && "move `from == to`");
    const auto piece = piece_at(move.from);
    assert(piece.has_value() && "move from empty square");
    spdlog::debug("move(piece={}, from={}, to={})", *piece, move.from, move.to);
    make_move({*piece, BitBoard{move.from}, BitBoard{move.to}});
}

void Board::make_move(BitBoardPieceMove piece_move)
{
    assert(piece_move.from.count() == 1);
    assert(piece_move.to.count() == 1);

    // en passant capture
    if (piece_move.piece.type == PieceType::pawn && en_passant_square_.test_any(piece_move.to)) {
        clear_piece({piece_move.from.to_position().x(), piece_move.to.to_position().y()});
    }

    // castling move
    if (piece_move.piece.type == PieceType::king) {
        castle(piece_move);
    }

    // normal move
    move_piece(piece_move);
    active_color_ = opposite_color(active_color_);

    update_en_passant_state(piece_move);
    update_castling_state(piece_move);
    move_history_.push_back(piece_move);
}

void Board::undo_previous_move()
{
    auto& move = move_history_.back();
    std::swap(move.to, move.from);
    move_history_.pop_back();
}

BitBoard Board::attacked_by_black_board() const
{
    BitBoard attacked_by_black;
    for (const auto position : black_.to_position_vector()) {
        attacked_by_black.set(attacking_bitboard(position));
    }
    return attacked_by_black;
}

BitBoard Board::attacked_by_white_board() const
{
    BitBoard attacked_by_white;
    for (const auto position : white_.to_position_vector()) {
        attacked_by_white.set(attacking_bitboard(position));
    }
    return attacked_by_white;
}

std::vector<Board::Position> Board::attacked_by_white() const
{
    return attacked_by_white_board().to_position_vector();
}

std::vector<Board::Position> Board::attacked_by_black() const
{
    return attacked_by_black_board().to_position_vector();
}

BitBoard Board::occupied_board() const
{
    return black_ | white_;
}

bool Board::occupied(const Position& position) const
{
    return occupied(BitBoard{position});
}

bool Board::occupied(const BitBoard position) const
{
    return occupied_board().test_all(position);
}

std::optional<PieceColor> Board::piece_color_at(const Position& position) const
{
    return piece_color_at(BitBoard{position});
}

bool Board::is_black(const BitBoard position) const
{
    return black_.test_all(position);
}

bool Board::is_white(const BitBoard position) const
{
    return white_.test_all(position);
}

std::optional<PieceColor> Board::piece_color_at(const BitBoard position) const
{
    if (is_black(position)) {
        return PieceColor::black;
    }
    if (is_white(position)) {
        return PieceColor::white;
    }
    return {};
}

std::optional<PieceType> Board::piece_type_at(const Position& position) const
{
    return piece_type_at(BitBoard{position});
}

bool Board::is_pawn(const BitBoard position) const
{
    return pawns_.test_all(position);
}

bool Board::is_knight(const BitBoard position) const
{
    return knights_.test_all(position);
}

bool Board::is_bishop(const BitBoard position) const
{
    return bishops_.test_all(position);
}

bool Board::is_rook(const BitBoard position) const
{
    return rooks_.test_all(position);
}

bool Board::is_queen(const BitBoard position) const
{
    return queens_.test_all(position);
}

bool Board::is_king(const BitBoard position) const
{
    return kings_.test_all(position);
}

std::optional<PieceType> Board::piece_type_at(const BitBoard position) const
{
    if (is_pawn(position)) {
        return PieceType::pawn;
    }
    if (is_knight(position)) {
        return PieceType::knight;
    }
    if (is_bishop(position)) {
        return PieceType::bishop;
    }
    if (is_rook(position)) {
        return PieceType::rook;
    }
    if (is_queen(position)) {
        return PieceType::queen;
    }
    if (is_king(position)) {
        return PieceType::king;
    }
    return {};
}

std::optional<Piece> Board::piece_at(const Position& position) const
{
    return piece_at(BitBoard{position});
}

std::optional<Piece> Board::piece_at(const BitBoard position) const
{
    const auto piece_color = piece_color_at(position);
    const auto piece_type = piece_type_at(position);
    if (!piece_color.has_value() || !piece_type.has_value()) {
        return {};
    }
    return Piece{.color = *piece_color, .type = *piece_type};
}

Piece Board::piece_at_checked(const BitBoard position) const
{
    const auto piece = piece_at(position);
    assert(piece.has_value());
    return *piece;
}

// todo: update color boards
void Board::move_pawn(const BitBoardMove move)
{
    pawns_.set(move.to);
    pawns_.clear(move.from);
}

void Board::move_knight(const BitBoardMove move)
{
    knights_.set(move.to);
    knights_.clear(move.from);
}

void Board::move_bishop(const BitBoardMove move)
{
    bishops_.set(move.to);
    bishops_.clear(move.from);
}

void Board::move_rook(const BitBoardMove move)
{
    rooks_.set(move.to);
    rooks_.clear(move.from);
}

void Board::move_queen(const BitBoardMove move)
{
    queens_.set(move.to);
    queens_.clear(move.from);
}

void Board::move_king(const BitBoardMove move)
{
    kings_.set(move.to);
    kings_.clear(move.from);
}

void Board::move_white(const BitBoardMove move)
{
    white_.set(move.to);
    white_.clear(move.from);
}

void Board::move_black(const BitBoardMove move)
{
    black_.set(move.to);
    black_.clear(move.from);
}

bool Board::is_valid_move(const Move move) const
{
    return valid_moves_bitboard(move.from).test(move.to);
}

bool Board::is_promotion_move(const Move move) const
{
    const auto piece = piece_at(move.from);
    assert(piece.has_value() && "move from empty square");

    if (piece->type != PieceType::pawn) {
        return false;
    }
    switch (piece->color) {
    case PieceColor::black:
        return move.to.x() == white_piece_row;
    case PieceColor::white:
        return move.to.x() == black_piece_row;
    }
}

std::vector<Board::Position> Board::valid_moves_vector(const Position from)
{
    return valid_moves_bitboard(from).to_position_vector();
}

std::set<Board::Position> Board::valid_moves_set(const Position from)
{
    return valid_moves_bitboard(from).to_position_set();
}

PieceColor Board::active_color() const
{
    return active_color_;
}

bool Board::is_active_piece(const Position& position) const
{
    return active_color_board().test(position);
}

bool Board::is_pawn_start_square(const Position from) const
{
    const auto pawn_row = piece_color_at(from) == PieceColor::black ? black_pawn_row : white_pawn_row;
    return from.x() == pawn_row;
}

BitBoard Board::pawn_moves(const Position from, const PieceColor color) const
{
    assert(is_pawn(BitBoard{from}) && "not a pawn");
    const auto from_board = BitBoard{from};
    const auto n_spaces = is_pawn_start_square(from) ? 2 : 1;
    BitBoard forward_moves;
    switch (color) {
    case PieceColor::white:
        forward_moves = sliding_moves<up>(from_board, n_spaces).clear(occupied_board());
        break;
    case PieceColor::black:
        forward_moves = sliding_moves<down>(from_board, n_spaces).clear(occupied_board());
        break;
    }
    return forward_moves | pawn_attacking_moves(from, color);
}

BitBoard Board::pawn_attacking_moves(const Position from, const PieceColor color) const
{
    assert(is_pawn(BitBoard{from}) && "not a pawn");
    BitBoard attacking_moves = pawn_attacking_squares(from, color);
    switch (color) {
    case PieceColor::white:
        attacking_moves &= (black_ | en_passant_square_);
        break;
    case PieceColor::black:
        attacking_moves &= (white_ | en_passant_square_);
        break;
    }
    return attacking_moves;
}

BitBoard Board::pawn_attacking_squares(const Position from, const PieceColor color) const
{
    assert(is_pawn(BitBoard{from}) && "not a pawn");
    const auto from_board = BitBoard{from};
    BitBoard attacking_squares;
    switch (color) {
    case PieceColor::white:
        attacking_squares = BitBoard::shift<upright>(from_board) | BitBoard::shift<upleft>(from_board);
        break;
    case PieceColor::black:
        attacking_squares = BitBoard::shift<downright>(from_board) | BitBoard::shift<downleft>(from_board);
        break;
    }
    return attacking_squares;
}

BitBoard Board::knight_moves(const Position from, const PieceColor color) const
{
    // 01010000
    // 10001000
    // 00000000
    // 10001000
    // 01010000
    // 00000000
    // 00000000
    // 00000000
    static constexpr BitBoard knight_moves_board{0x50'88'00'88'50'00'00'00};
    static const Position knight_moves_origin{2, 2};

    assert(is_knight(BitBoard{from}) && "not a knight");
    const Position offset = from - knight_moves_origin;
    return BitBoard{knight_moves_board}.shift_assign(offset);
}

BitBoard Board::bishop_moves(const Position from, const PieceColor color) const
{
    static constexpr std::array<Direction, 4> directions = {
        Direction::upright, Direction::upleft, Direction::downleft, Direction::downright};
    assert(is_bishop(BitBoard{from}) && "not a bishop");
    return sliding_moves(directions, from);
}

BitBoard Board::rook_moves(const Position from, const PieceColor color) const
{
    static constexpr std::array<Direction, 4> directions = {
        Direction::right, Direction::up, Direction::left, Direction::down};
    assert(is_rook(BitBoard{from}) && "not a rook");
    return sliding_moves(directions, from);
}

BitBoard Board::queen_moves(const Position from, const PieceColor color) const
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
    assert(is_queen(BitBoard{from}) && "not a queen");
    return sliding_moves(directions, from);
}

bool Board::white_can_castle_kingside() const
{
    static constexpr BitBoard between_squares{0x00'00'00'00'00'00'00'06};
    static constexpr BitBoard king_squares{0x00'00'00'00'00'00'00'0E};
    return !white_kingside_castle_piece_moved_ && white_can_castle(between_squares, king_squares);
}

bool Board::white_can_castle_queenside() const
{
    static constexpr BitBoard between_squares{0x00'00'00'00'00'00'00'70};
    static constexpr BitBoard king_squares{0x00'00'00'00'00'00'00'38};
    return !white_queenside_castle_piece_moved_ && white_can_castle(between_squares, king_squares);
}

bool Board::black_can_castle_kingside() const
{
    static constexpr BitBoard between_squares{0x06'00'00'00'00'00'00'00};
    static constexpr BitBoard king_squares{0x0E'00'00'00'00'00'00'00};
    return !black_kingside_castle_piece_moved_ && black_can_castle(between_squares, king_squares);
}

bool Board::black_can_castle_queenside() const
{
    static constexpr BitBoard between_squares{0x70'00'00'00'00'00'00'00};
    static constexpr BitBoard king_squares{0x38'00'00'00'00'00'00'00};
    return !black_queenside_castle_piece_moved_ && black_can_castle(between_squares, king_squares);
}

bool Board::black_can_castle(const BitBoard between_squares, const BitBoard king_squares) const
{
    return (!between_squares.test_any(occupied_board()) && !king_squares.test_any(attacked_by_white_board()));
}

bool Board::white_can_castle(const BitBoard between_squares, const BitBoard king_squares) const
{
    return (!between_squares.test_any(occupied_board()) && !king_squares.test_any(attacked_by_black_board()));
}

BitBoard Board::white_king_castling_moves() const
{
    return (white_can_castle_kingside() ? white_castle_kingside_king_move : BitBoard{}) |
           (white_can_castle_queenside() ? white_castle_queenside_king_move : BitBoard{});
}

BitBoard Board::black_king_castling_moves() const
{
    return (black_can_castle_kingside() ? black_castle_kingside_king_move : BitBoard{}) |
           (black_can_castle_queenside() ? black_castle_queenside_king_move : BitBoard{});
}

BitBoard Board::king_castling_moves(const PieceColor color) const
{
    switch (color) {
    case PieceColor::white:
        return white_king_castling_moves();
    case PieceColor::black:
        return black_king_castling_moves();
    }
}

BitBoard Board::king_standard_moves(const Position from, const PieceColor color) const
{
    assert(is_king(BitBoard{from}) && "not a king");
    return BitBoard::neighbors_cardinal_and_diagonal(from).clear(board_of_color(color));
}

BitBoard Board::king_moves(const Position from, const PieceColor color) const
{
    assert(is_king(BitBoard{from}) && "not a king");
    return king_standard_moves(from, color) | king_castling_moves(color);
}

BitBoard Board::valid_moves_bitboard(const Position from) const
{
    BitBoard moves;
    const auto piece = piece_at(from);
    if (!piece.has_value()) {
        return moves;
    }
    switch (piece->type) {
    case PieceType::pawn:
        moves = pawn_moves(from, piece->color);
        break;
    case PieceType::knight:
        moves = knight_moves(from, piece->color);
        break;
    case PieceType::bishop:
        moves = bishop_moves(from, piece->color);
        break;
    case PieceType::rook:
        moves = rook_moves(from, piece->color);
        break;
    case PieceType::queen:
        moves = queen_moves(from, piece->color);
        break;
    case PieceType::king:
        moves = king_moves(from, piece->color);
        break;
    }
    return moves.clear(board_of_color(piece->color));
}

BitBoard Board::attacking_bitboard(const Position from) const
{
    BitBoard moves;
    const auto piece = piece_at(from);
    if (!piece.has_value()) {
        return moves;
    }
    switch (piece->type) {
    case PieceType::pawn:
        moves = pawn_attacking_squares(from, piece->color);
        break;
    case PieceType::knight:
        moves = knight_moves(from, piece->color);
        break;
    case PieceType::bishop:
        moves = bishop_moves(from, piece->color);
        break;
    case PieceType::rook:
        moves = rook_moves(from, piece->color);
        break;
    case PieceType::queen:
        moves = queen_moves(from, piece->color);
        break;
    case PieceType::king:
        moves = king_standard_moves(from, piece->color);
        break;
    }
    return moves.clear(board_of_color(piece->color));
}

BitBoard Board::sliding_moves(const Direction direction, const BitBoard from, const size_t range) const
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

BitBoard Board::board_of_color(PieceColor color) const
{
    return color == PieceColor::black ? black_ : white_;
}

BitBoard& Board::board_of_color(PieceColor color)
{
    return color == PieceColor::black ? black_ : white_;
}

BitBoard& Board::active_color_board()
{
    return board_of_color(active_color_);
}

BitBoard Board::active_color_board() const
{
    return board_of_color(active_color_);
}

BitBoard& Board::inactive_color_board()
{
    return board_of_color(opposite_color(active_color_));
}

BitBoard Board::inactive_color_board() const
{
    return board_of_color(opposite_color(active_color_));
}

Board Board::make_standard_setup_board()
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

    Board pieces;
    for (Position::dimension_type column = 0; column < BitBoard::board_size; ++column) {
        pieces.set_piece({PieceColor::black, back_row[column]}, {black_piece_row, column});
        pieces.set_piece({PieceColor::black, PieceType::pawn}, {black_pawn_row, column});
        pieces.set_piece({PieceColor::white, PieceType::pawn}, {white_pawn_row, column});
        pieces.set_piece({PieceColor::white, back_row[column]}, {white_piece_row, column});
    }

    return pieces;
}

} // namespace chess