#include "pieces.h"

#include "bit_board_formatter.h"

#include <spdlog/spdlog.h>

#include <cassert>
#include <exception>

namespace chess {

const std::map<PieceColor, std::string> piece_color_names{
    {PieceColor::white, "white"},
    {PieceColor::black, "black"},
};

const std::map<PieceType, std::string> piece_type_names{
    {PieceType::pawn, "pawn"},
    {PieceType::knight, "knight"},
    {PieceType::bishop, "bishop"},
    {PieceType::rook, "rook"},
    {PieceType::queen, "queen"},
    {PieceType::king, "king"},
};

void BoardPieces::clear_pieces(const BitBoard board)
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

void BoardPieces::set_pieces(const Piece piece, const BitBoard positions)
{
    clear_pieces(positions);

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

void BoardPieces::clear_piece(const Position& position)
{
    clear_squares_attacked_by(position);
    clear_pieces(position);
}

void BoardPieces::set_piece(const Piece piece, const Position& position)
{
    set_pieces(piece, position);
    set_squares_attacked_by(position);
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
    assert(from != to && "move `from == to`");
    const auto piece = piece_at(from);
    if (!piece.has_value()) {
        throw std::runtime_error("move from empty square");
    }
    const auto move = Move{*piece, from, to};

    if (move.piece.type == PieceType::pawn && en_passant_square_.test(move.to)) {
        clear_piece({move.from.x(), move.to.y()});
    }

    set_piece(move.piece, move.to);
    clear_piece(move.from);

    update_en_passant_state(move);
    update_castling_state(move);
    active_color_ = opposite_color(active_color_);
    move_history_.push_back(move);
}

void BoardPieces::clear_squares_attacked_by(const Position& position)
{
    assert(occupied(position) && "square not occupied");
    for (const auto attacked_square : valid_moves_bitboard(position).to_position_vector()) {
        attacked_by_[attacked_square].clear(position);
    }
}

void BoardPieces::set_squares_attacked_by(const Position& position)
{
    assert(occupied(position) && "square not occupied");
    for (const auto attacked_square : valid_moves_bitboard(position).to_position_vector()) {
        attacked_by_[attacked_square].set(position);
    }
}

[[nodiscard]] BitBoard BoardPieces::occupied_board() const
{
    return black_ | white_;
}

[[nodiscard]] bool BoardPieces::occupied(const Position& position) const
{
    return occupied(BitBoard{position});
}

[[nodiscard]] bool BoardPieces::occupied(const BitBoard position) const
{
    return occupied_board().test_all(position);
}

[[nodiscard]] std::optional<PieceColor> BoardPieces::piece_color_at(const Position& position) const
{
    return piece_color_at(BitBoard{position});
}

[[nodiscard]] bool BoardPieces::is_black(const BitBoard position) const
{
    return black_.test_all(position);
}

[[nodiscard]] bool BoardPieces::is_white(const BitBoard position) const
{
    return white_.test_all(position);
}

[[nodiscard]] std::optional<PieceColor> BoardPieces::piece_color_at(const BitBoard position) const
{
    if (is_black(position)) {
        return PieceColor::black;
    }
    if (is_white(position)) {
        return PieceColor::white;
    }
    return {};
}

[[nodiscard]] std::optional<PieceType> BoardPieces::piece_type_at(const Position& position) const
{
    return piece_type_at(BitBoard{position});
}

[[nodiscard]] bool BoardPieces::is_pawn(const BitBoard position) const
{
    return pawns_.test_all(position);
}

[[nodiscard]] bool BoardPieces::is_knight(const BitBoard position) const
{
    return knights_.test_all(position);
}

[[nodiscard]] bool BoardPieces::is_bishop(const BitBoard position) const
{
    return bishops_.test_all(position);
}

[[nodiscard]] bool BoardPieces::is_rook(const BitBoard position) const
{
    return rooks_.test_all(position);
}

[[nodiscard]] bool BoardPieces::is_queen(const BitBoard position) const
{
    return queens_.test_all(position);
}

[[nodiscard]] bool BoardPieces::is_king(const BitBoard position) const
{
    return kings_.test_all(position);
}

[[nodiscard]] std::optional<PieceType> BoardPieces::piece_type_at(const BitBoard position) const
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

[[nodiscard]] std::optional<Piece> BoardPieces::piece_at(const Position& position) const
{
    const auto piece_color = piece_color_at(position);
    const auto piece_type = piece_type_at(position);
    if (!piece_color.has_value() || !piece_type.has_value()) {
        return {};
    }
    return Piece{.color = *piece_color, .type = *piece_type};
}

[[nodiscard]] bool BoardPieces::is_valid_move(const Position& from, const Position& to)
{
    return valid_moves_bitboard(from).test(to);
}

[[nodiscard]] BitBoard BoardPieces::valid_moves(const Position& from)
{
    return valid_moves_bitboard(from);
}

[[nodiscard]] PieceColor BoardPieces::active_color() const
{
    return active_color_;
}

[[nodiscard]] bool BoardPieces::is_active_piece(const Position& position) const
{
    return active_color_board().test(position);
}

bool BoardPieces::on_pawn_start_square(const Position& from) const
{
    const auto pawn_row = piece_color_at(from) == PieceColor::black ? black_pawn_row : white_pawn_row;
    return from.x() == pawn_row;
}

[[nodiscard]] BitBoard BoardPieces::pawn_moves(const Position& from) const
{
    const auto from_board = BitBoard{from};
    const auto n_spaces = on_pawn_start_square(from) ? 2 : 1;
    BitBoard moves;
    BitBoard attacking_moves;
    switch (active_color_) {
    case PieceColor::white:
        moves = sliding_moves<up>(from, n_spaces);
        attacking_moves.set(BitBoard::shift<upright>(from_board));
        attacking_moves.set(BitBoard::shift<upleft>(from_board));
        attacking_moves.clear(~black_);
        attacking_moves &= en_passant_square_;
        break;
    case PieceColor::black:
        moves = sliding_moves<down>(from, n_spaces);
        attacking_moves.set(BitBoard::shift<downright>(from_board));
        attacking_moves.set(BitBoard::shift<downleft>(from_board));
        attacking_moves.clear(~white_);
        break;
    }
    moves.clear(occupied_board());
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
    moves.clear(active_color_board());
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
    moves.clear(active_color_board());
    moves.clear(from);
    return moves;
}

[[nodiscard]] BitBoard BoardPieces::valid_moves_bitboard(const Position& from) const
{
    BitBoard moves;
    const auto piece = piece_at(from);
    if (!piece.has_value() || piece->color != active_color_) {
        return moves;
    }
    switch (piece->type) {
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

BitBoard& BoardPieces::active_color_board()
{
    return active_color_ == PieceColor::black ? black_ : white_;
}

BitBoard BoardPieces::active_color_board() const
{
    return active_color_ == PieceColor::black ? black_ : white_;
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
        pieces.set_piece({PieceColor::black, PieceType::pawn}, {black_pawn_row, y});
        pieces.set_piece({PieceColor::white, PieceType::pawn}, {white_pawn_row, y});
        pieces.set_piece({PieceColor::white, back_row[y]}, {white_piece_row, y});
    }
    return pieces;
}

} // namespace chess
