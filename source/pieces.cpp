#include "pieces.h"

#include "bit_board_formatter.h"
#include "piece_formatter.h"

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

    spdlog::debug("set_pieces({}, {})", piece, positions);
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
    clear_pieces(position);
}

void BoardPieces::set_piece(const Piece piece, const Position& position)
{
    set_pieces(piece, BitBoard{position});
}

void BoardPieces::update_en_passant_state(const PieceMove move)
{
    en_passant_square_.reset_all();
    if (move.piece.type == PieceType::pawn) {
        if (Position::chebyshev_distance(move.from, move.to) == 2) {
            en_passant_square_.set((move.from + move.to) / 2);
        }
    }
}

void BoardPieces::update_castling_state(const PieceMove move)
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

void BoardPieces::move_piece(const Position from, const Position to)
{
    assert(from != to && "move `from == to`");
    const auto piece = piece_at(from);
    assert(piece.has_value() && "move from empty square");
    const auto move = PieceMove{*piece, from, to};

    // en passant capture
    if (move.piece.type == PieceType::pawn && en_passant_square_.test(move.to)) {
        clear_piece({move.from.x(), move.to.y()});
    }

    // castling move
    if (move.piece.type == PieceType::king) {
        PieceMove rook_move;
        switch (active_color_) {
        case PieceColor::black:
            if (!black_king_castling_moves(move.from).test(move.to)) {
                break;
            }
            if (black_castle_kingside_king_move == move.from) {
                const auto rook = piece_at(black_kingside_rook_position);
                assert(rook.has_value());
                rook_move = PieceMove{*rook, black_kingside_rook_position, black_castle_kingside_rook_move};
            }
            if (black_castle_queenside_king_move == move.from) {
                const auto rook = piece_at(black_queenside_rook_position);
                assert(rook.has_value());
                rook_move = PieceMove{*rook, black_queenside_rook_position, black_castle_queenside_rook_move};
            }
            break;
        case PieceColor::white:
            if (!white_king_castling_moves(move.from).test(move.to)) {
                break;
            }
            if (white_castle_kingside_king_move == move.from) {
                const auto rook = piece_at(white_kingside_rook_position);
                assert(rook.has_value());
                rook_move = PieceMove{*rook, white_kingside_rook_position, white_castle_kingside_rook_move};
            }
            if (white_castle_queenside_king_move == move.from) {
                const auto rook = piece_at(white_queenside_rook_position);
                assert(rook.has_value());
                rook_move = PieceMove{*rook, white_queenside_rook_position, white_castle_queenside_rook_move};
            }
            break;
        }
        set_piece(rook_move.piece, rook_move.to);
        clear_piece(rook_move.from);
    }

    // normal move
    set_piece(move.piece, move.to);
    clear_piece(move.from);
    active_color_ = opposite_color(active_color_);

    update_en_passant_state(move);
    update_castling_state(move);
    move_history_.push_back(move);
}

void BoardPieces::clear_squares_attacked_by(const Position& position)
{
    assert(occupied(position) && "square not occupied");
}

void BoardPieces::set_squares_attacked_by(const Position& position)
{
    assert(occupied(position) && "square not occupied");
}

[[nodiscard]] BitBoard BoardPieces::attacked_board() const
{
    return attacked_;
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
    return piece_at(BitBoard{position});
}

[[nodiscard]] std::optional<Piece> BoardPieces::piece_at(const BitBoard position) const
{
    const auto piece_color = piece_color_at(position);
    const auto piece_type = piece_type_at(position);
    if (!piece_color.has_value() || !piece_type.has_value()) {
        return {};
    }
    return Piece{.color = *piece_color, .type = *piece_type};
}

// todo: update color boards
void BoardPieces::move_pawn(const BitBoard from, const BitBoard to)
{
    pawns_.set(to);
    pawns_.clear(from);
}

void BoardPieces::move_knight(const BitBoard from, const BitBoard to)
{
    knights_.set(to);
    knights_.clear(from);
}

void BoardPieces::move_bishop(const BitBoard from, const BitBoard to)
{
    bishops_.set(to);
    bishops_.clear(from);
}

void BoardPieces::move_rook(const BitBoard from, const BitBoard to)
{
    rooks_.set(to);
    rooks_.clear(from);
}

void BoardPieces::move_queen(const BitBoard from, const BitBoard to)
{
    queens_.set(to);
    queens_.clear(from);
}

void BoardPieces::move_king(const BitBoard from, const BitBoard to)
{
    kings_.set(to);
    kings_.clear(from);
}

void BoardPieces::move_white(const BitBoard from, const BitBoard to)
{
    white_.set(to);
    white_.clear(from);
}

void BoardPieces::move_black(const BitBoard from, const BitBoard to)
{
    black_.set(to);
    black_.clear(from);
}

[[nodiscard]] bool BoardPieces::is_valid_move(const Position from, const Position to)
{
    return valid_moves_bitboard(from).test(to);
}

[[nodiscard]] BitBoard BoardPieces::valid_moves(const Position from)
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

bool BoardPieces::is_pawn_start_square(const Position from) const
{
    const auto pawn_row = piece_color_at(from) == PieceColor::black ? black_pawn_row : white_pawn_row;
    return from.x() == pawn_row;
}

[[nodiscard]] BitBoard BoardPieces::pawn_moves(const Position from) const
{
    assert(is_pawn(from) && "not a pawn");
    const auto from_board = BitBoard{from};
    const auto n_spaces = is_pawn_start_square(from) ? 2 : 1;
    BitBoard forward_moves;
    BitBoard attacking_moves;
    switch (active_color_) {
    case PieceColor::white:
        forward_moves = sliding_moves<up>(from, n_spaces);
        attacking_moves = BitBoard::shift<upright>(from_board) | BitBoard::shift<upleft>(from_board);
        attacking_moves &= (black_ | en_passant_square_);
        break;
    case PieceColor::black:
        forward_moves = sliding_moves<down>(from, n_spaces);
        attacking_moves = BitBoard::shift<downright>(from_board) | BitBoard::shift<downleft>(from_board);
        attacking_moves &= (white_ | en_passant_square_);
        break;
    }
    return forward_moves | attacking_moves;
}

[[nodiscard]] BitBoard BoardPieces::knight_moves(const Position from) const
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

    assert(is_knight(from) && "not a knight");
    const Position offset = from - knight_moves_origin;
    auto moves = BitBoard{knight_moves_board}.shift_assign(offset);
    moves.clear(active_color_board());
    return moves;
}

[[nodiscard]] BitBoard BoardPieces::bishop_moves(const Position from) const
{
    static constexpr std::array<Direction, 4> directions = {
        Direction::upright, Direction::upleft, Direction::downleft, Direction::downright};
    assert(is_bishop(from) && "not a bishop");
    return sliding_moves(from, directions);
}

[[nodiscard]] BitBoard BoardPieces::rook_moves(const Position from) const
{
    static constexpr std::array<Direction, 4> directions = {
        Direction::right, Direction::up, Direction::left, Direction::down};
    assert(is_rook(from) && "not a rook");
    return sliding_moves(from, directions);
}

[[nodiscard]] BitBoard BoardPieces::queen_moves(const Position from) const
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
    assert(is_queen(from) && "not a queen");
    return sliding_moves(from, directions);
}

bool BoardPieces::white_can_castle_kingside() const
{
    static constexpr BitBoard between_squares{0x00'00'00'00'00'00'00'06};
    static constexpr BitBoard king_squares{0x00'00'00'00'00'00'00'06};
    return can_castle(between_squares, king_squares);
}

bool BoardPieces::white_can_castle_queenside() const
{
    static constexpr BitBoard between_squares{0x00'00'00'00'00'00'00'70};
    static constexpr BitBoard king_squares{0x00'00'00'00'00'00'00'30};
    return can_castle(between_squares, king_squares);
}

bool BoardPieces::black_can_castle_kingside() const
{
    static constexpr BitBoard between_squares{0x06'00'00'00'00'00'00'00};
    static constexpr BitBoard king_squares{0x06'00'00'00'00'00'00'00};
    return can_castle(between_squares, king_squares);
}

bool BoardPieces::black_can_castle_queenside() const
{
    static constexpr BitBoard between_squares{0x70'00'00'00'00'00'00'00};
    static constexpr BitBoard king_squares{0x30'00'00'00'00'00'00'00};
    return can_castle(between_squares, king_squares);
}

bool BoardPieces::can_castle(const BitBoard between_squares, const BitBoard king_squares) const
{
    return (!between_squares.test_any(occupied_board()) && !king_squares.test_any(attacked_board()));
}

[[nodiscard]] BitBoard BoardPieces::white_king_castling_moves(const Position from) const
{
    assert(is_king(from) && "not a king");
    return (white_can_castle_kingside() ? white_castle_kingside_king_move : BitBoard{}) |
           (white_can_castle_queenside() ? white_castle_queenside_king_move : BitBoard{});
}

[[nodiscard]] BitBoard BoardPieces::black_king_castling_moves(const Position from) const
{
    assert(is_king(from) && "not a king");
    return (black_can_castle_kingside() ? black_castle_kingside_king_move : BitBoard{}) |
           (black_can_castle_queenside() ? black_castle_queenside_king_move : BitBoard{});
}

[[nodiscard]] BitBoard BoardPieces::king_castling_moves(const Position from) const
{
    assert(is_king(from) && "not a king");
    switch (active_color_) {
    case PieceColor::white:
        return white_king_castling_moves(from);
    case PieceColor::black:
        return black_king_castling_moves(from);
    }
}

[[nodiscard]] BitBoard BoardPieces::king_moves(const Position from) const
{
    assert(is_king(from) && "not a king");
    auto moves = BitBoard::neighbors_cardinal_and_diagonal(from);
    moves.clear(active_color_board());
    return moves | king_castling_moves(from);
}

[[nodiscard]] BitBoard BoardPieces::valid_moves_bitboard(const Position from) const
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

BitBoard BoardPieces::sliding_moves(const BitBoard from, const Direction direction, const size_t range) const
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

BitBoard BoardPieces::board_of_color(PieceColor color) const
{
    return color == PieceColor::black ? black_ : white_;
}

BitBoard& BoardPieces::board_of_color(PieceColor color)
{
    return color == PieceColor::black ? black_ : white_;
}

BitBoard& BoardPieces::active_color_board()
{
    return board_of_color(active_color_);
}

BitBoard BoardPieces::active_color_board() const
{
    return board_of_color(active_color_);
}

BitBoard& BoardPieces::inactive_color_board()
{
    return board_of_color(opposite_color(active_color_));
}

BitBoard BoardPieces::inactive_color_board() const
{
    return board_of_color(opposite_color(active_color_));
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
