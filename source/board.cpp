#include "board.h"

#include "vec2.h"
#include "vec2_formatter.h"

#include <spdlog/fmt/fmt.h>

#include <cassert>

#include <algorithm>
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
    set_pieces(move.piece, move.to);
    clear_pieces(move.from);
}

void Board::white_castle(const BitBoardPieceMove king_move)
{
    const auto white_moves = king_castling_moves<PieceColor::white>();
    if ((white_moves & white_castle_kingside_king_move) == king_move.to) {
        move_piece({pieces::white_rook, white_kingside_rook_position, white_castle_kingside_rook_move});
    }
    if ((white_moves & white_castle_queenside_king_move) == king_move.to) {
        move_piece({pieces::white_rook, white_queenside_rook_position, white_castle_queenside_rook_move});
    }
}

void Board::black_castle(const BitBoardPieceMove king_move)
{
    const auto black_moves = king_castling_moves<PieceColor::black>();
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

void Board::make_move(const Move move, std::optional<PieceType> promotion_selection)
{
    make_move({BitBoard{move.from}, BitBoard{move.to}}, promotion_selection);
}

void Board::make_move(const BitBoardMove move, std::optional<PieceType> promotion_selection)
{
    make_move({piece_at_checked(move.from), move.from, move.to}, promotion_selection);
}

void Board::make_move(const BitBoardPieceMove piece_move, std::optional<PieceType> promotion_selection)
{
    assert(piece_move.from != piece_move.to && "move `from == to`");
    assert(piece_move.from.count() == 1);
    assert(piece_move.to.count() == 1);

    history_.emplace_back(*this);

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
}

void Board::undo_previous_move()
{
    set_state(history_.back());
    history_.pop_back();
}

BitBoard Board::attacked_by_color(const PieceColor color) const
{
    return (color == PieceColor::black) ? attacked_by<PieceColor::black>() : attacked_by<PieceColor::white>();
}

bool Board::is_color_in_check(const PieceColor color) const
{
    return (color == PieceColor::black) ? is_in_check<PieceColor::black>() : is_in_check<PieceColor::white>();
}

bool Board::is_active_in_check() const
{
    return is_color_in_check(active_color());
}

bool Board::test_move_for_self_check(const BitBoardMove& move) const
{
    Board test_board{*this};
    test_board.make_move(move);
    return test_board.is_color_in_check(test_board.inactive_color());
}

bool Board::is_in_checkmate() const
{
    return is_color_in_check(active_color()) && !has_valid_move();
}

bool Board::is_in_stalemate() const
{
    return !is_color_in_check(active_color()) && !has_valid_move();
}

Board::Position Board::active_king_position() const
{
    return (active_color_board() & kings_).to_position();
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

std::optional<PieceColor> Board::piece_color_at(const BitBoard position) const
{
    if (black_.test_all(position)) {
        return PieceColor::black;
    }
    if (white_.test_all(position)) {
        return PieceColor::white;
    }
    return {};
}

std::optional<PieceType> Board::piece_type_at(const Position& position) const
{
    return piece_type_at(BitBoard{position});
}

std::optional<PieceType> Board::piece_type_at(const BitBoard position) const
{
    if (pawns_.test_all(position)) {
        return PieceType::pawn;
    }
    if (knights_.test_all(position)) {
        return PieceType::knight;
    }
    if (bishops_.test_all(position)) {
        return PieceType::bishop;
    }
    if (rooks_.test_all(position)) {
        return PieceType::rook;
    }
    if (queens_.test_all(position)) {
        return PieceType::queen;
    }
    if (kings_.test_all(position)) {
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

Piece Board::piece_at_checked(const Position& position) const
{
    return piece_at_checked(BitBoard{position});
}

Piece Board::piece_at_checked(const BitBoard position) const
{
    const auto piece = piece_at(position);
    if (!piece.has_value()) {
        throw std::runtime_error(fmt::format("no piece at position {}", position.to_position()));
    }
    return *piece;
}

bool Board::is_promotion_move(const Move move) const
{
    const auto piece = piece_at_checked(move.from);
    return (piece.color == PieceColor::black) ? is_promotion_move<PieceColor::black>(BitBoardMove::from_move(move))
                                              : is_promotion_move<PieceColor::white>(BitBoardMove::from_move(move));
}

[[nodiscard]] BitBoard Board::valid_moves_bitboard(BitBoard from) const
{
    return (active_color() == PieceColor::black) ? valid_moves_bitboard<PieceColor::black>(from)
                                                 : valid_moves_bitboard<PieceColor::white>(from);
}

std::vector<Board::Position> Board::valid_moves_vector(const Position from)
{
    return valid_moves_bitboard(BitBoard{from}).to_position_vector();
}

std::set<Board::Position> Board::valid_moves_set(const Position from)
{
    return valid_moves_bitboard(BitBoard{from}).to_position_set();
}

PieceColor Board::active_color() const
{
    return active_color_;
}

PieceColor Board::inactive_color() const
{
    return opposite_color(active_color_);
}

bool Board::is_active_piece(const Position& position) const
{
    return active_color_board().test(position);
}

BitBoard Board::knight_moves(const BitBoard from) const
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

    assert(knights_.test_all(from) && "not a knight");
    const auto offset = from.to_position() - knight_moves_origin;
    return BitBoard::shift(knight_moves_board, offset);
}

BitBoard Board::bishop_moves(const BitBoard from) const
{
    static constexpr std::array<Direction, 4> directions = {
        Direction::upright, Direction::upleft, Direction::downleft, Direction::downright};
    assert(bishops_.test_all(from) && "not a bishop");
    return sliding_moves(directions, from);
}

BitBoard Board::rook_moves(const BitBoard from) const
{
    static constexpr std::array<Direction, 4> directions = {
        Direction::right, Direction::up, Direction::left, Direction::down};
    assert(rooks_.test_all(from) && "not a rook");
    return sliding_moves(directions, from);
}

BitBoard Board::queen_moves(const BitBoard from) const
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
    assert(queens_.test_any(from) && "not a queen");
    return sliding_moves(directions, from);
}

bool Board::white_can_castle_kingside() const
{
    static constexpr BitBoard between_squares{0x00'00'00'00'00'00'00'06};
    static constexpr BitBoard king_squares{0x00'00'00'00'00'00'00'0E};
    return !white_kingside_castle_piece_moved_ && can_castle<PieceColor::white>(between_squares, king_squares);
}

bool Board::white_can_castle_queenside() const
{
    static constexpr BitBoard between_squares{0x00'00'00'00'00'00'00'70};
    static constexpr BitBoard king_squares{0x00'00'00'00'00'00'00'38};
    return !white_queenside_castle_piece_moved_ && can_castle<PieceColor::white>(between_squares, king_squares);
}

bool Board::black_can_castle_kingside() const
{
    static constexpr BitBoard between_squares{0x06'00'00'00'00'00'00'00};
    static constexpr BitBoard king_squares{0x0E'00'00'00'00'00'00'00};
    return !black_kingside_castle_piece_moved_ && can_castle<PieceColor::black>(between_squares, king_squares);
}

bool Board::black_can_castle_queenside() const
{
    static constexpr BitBoard between_squares{0x70'00'00'00'00'00'00'00};
    static constexpr BitBoard king_squares{0x38'00'00'00'00'00'00'00};
    return !black_queenside_castle_piece_moved_ && can_castle<PieceColor::black>(between_squares, king_squares);
}

BitBoard Board::king_standard_moves(const BitBoard from) const
{
    assert(kings_.test_all(from) && "not a king");
    return BitBoard::neighbors_cardinal_and_diagonal(from);
}

bool Board::has_valid_move() const
{
    return std::ranges::any_of(active_color_board().to_bitboard_vector(), [this](const auto from) {
        return !valid_moves_bitboard(from).empty();
    });
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
    default:
        assert(!"invalid direction");
        return BitBoard{};
    }
}

void Board::set_state(const BoardState& state)
{
    en_passant_square_ = state.en_passant_square;
    pawns_ = state.pawns;
    knights_ = state.knights;
    bishops_ = state.bishops;
    rooks_ = state.rooks;
    queens_ = state.queens;
    kings_ = state.kings;
    black_ = state.black;
    white_ = state.white;
    active_color_ = state.active_color;
    black_queenside_castle_piece_moved_ = state.black_queenside_castle_piece_moved;
    black_kingside_castle_piece_moved_ = state.black_kingside_castle_piece_moved;
    white_queenside_castle_piece_moved_ = state.white_queenside_castle_piece_moved;
    white_kingside_castle_piece_moved_ = state.white_kingside_castle_piece_moved;
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
        pieces.set_piece({PieceColor::black, back_row[column]}, {black_piece_row_index, column});
        pieces.set_piece({PieceColor::black, PieceType::pawn}, {black_pawn_row_index, column});
        pieces.set_piece({PieceColor::white, PieceType::pawn}, {white_pawn_row_index, column});
        pieces.set_piece({PieceColor::white, back_row[column]}, {white_piece_row_index, column});
    }

    return pieces;
}

} // namespace chess
