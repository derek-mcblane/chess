#include "game.h"

namespace chess {

std::optional<Piece> GameBoard::piece_at(const Position position) const
{
    return pieces_.at(position);
}

void GameBoard::update_en_passant_state(const Piece piece, const BitBoardMove move)
{
    en_passant_square_.clear_all();
    if (piece.type == PieceType::pawn) {
        const auto from_position = move.from.to_position();
        const auto to_position = move.to.to_position();
        if (Position::chebyshev_distance(from_position, to_position) == 2) {
            en_passant_square_.set((from_position + to_position) / 2);
        }
    }
}

void GameBoard::update_castling_state(const BitBoardMove move)
{
    if (move.from == GameBoard::black_queenside_rook_position) {
        black_queenside_castle_piece_moved_ = true;
    }
    if (move.from == GameBoard::black_kingside_rook_position) {
        black_kingside_castle_piece_moved_ = true;
    }
    if (move.from == GameBoard::black_king_position) {
        black_queenside_castle_piece_moved_ = true;
        black_kingside_castle_piece_moved_ = true;
    }
    if (move.from == GameBoard::white_queenside_rook_position) {
        white_queenside_castle_piece_moved_ = true;
    }
    if (move.from == GameBoard::white_kingside_rook_position) {
        white_kingside_castle_piece_moved_ = true;
    }
    if (move.from == GameBoard::white_king_position) {
        white_queenside_castle_piece_moved_ = true;
        white_kingside_castle_piece_moved_ = true;
    }
}

template <PieceColor Color>
bool GameBoard::is_castling_move(BitBoardMove move) const
{
    return king_castling_moves<Color>().test(move.to);
}

void GameBoard::white_castle(const BitBoardMove king_move)
{
    const auto white_moves = king_castling_moves<PieceColor::white>();
    if ((white_moves & white_castle_kingside_king_move) == king_move.to) {
        pieces_.move<pieces::white_rook>({white_kingside_rook_position, white_castle_kingside_rook_move});
    }
    if ((white_moves & white_castle_queenside_king_move) == king_move.to) {
        pieces_.move<pieces::white_rook>({white_queenside_rook_position, white_castle_queenside_rook_move});
    }
}

void GameBoard::black_castle(const BitBoardMove king_move)
{
    const auto black_moves = king_castling_moves<PieceColor::black>();
    if ((black_moves & black_castle_kingside_king_move) == king_move.to) {
        pieces_.move<pieces::black_rook>({black_kingside_rook_position, black_castle_kingside_rook_move});
    }
    if ((black_moves & black_castle_queenside_king_move) == king_move.to) {
        pieces_.move<pieces::black_rook>({black_queenside_rook_position, black_castle_queenside_rook_move});
    }
}

void GameBoard::castle(const Piece piece, const BitBoardMove king_move)
{
    switch (piece.color) {
    case PieceColor::black:
        black_castle(king_move);
    case PieceColor::white:
        white_castle(king_move);
        break;
    }
}

void GameBoard::make_move(const Move move, std::optional<PieceType> promotion_selection)
{
    make_move({BitBoard{move.from}, BitBoard{move.to}}, promotion_selection);
}

void GameBoard::make_move(const BitBoardMove move, std::optional<PieceType> promotion_selection)
{
    make_move(pieces_.at_checked(move.from), move, promotion_selection);
}

void GameBoard::make_move(const Piece piece, const BitBoardMove move, std::optional<PieceType> promotion_selection)
{
    assert(move.from.count() == 1U);
    assert(move.to.count() == 1U);
    if (move.from == move.to) {
        throw std::invalid_argument("move.from == move.to");
    }
    if (promotion_selection.has_value() &&
        (*promotion_selection == PieceType::pawn || *promotion_selection == PieceType::king)) {
        throw std::invalid_argument("invalid promotion selection");
    }

    history_.emplace_back(*this);

    // en passant capture
    if (piece.type == PieceType::pawn && en_passant_square_.test_any(move.to)) {
        pieces_.clear({move.from.to_position().x(), move.to.to_position().y()});
    }

    // castling move
    if (piece.type == PieceType::king) {
        castle(piece, move);
    }

    // normal move
    pieces_.move(piece, move);
    active_color_ = opposite_color(active_color_);

    update_en_passant_state(piece, move);
    update_castling_state(move);
}

void GameBoard::undo_previous_move()
{
    set_state(history_.back());
    history_.pop_back();
}

BitBoard GameBoard::attacked_by_color(const PieceColor color) const
{
    return (color == PieceColor::black) ? attacked_by<PieceColor::black>() : attacked_by<PieceColor::white>();
}

bool GameBoard::is_color_in_check(const PieceColor color) const
{
    return (color == PieceColor::black) ? is_in_check<PieceColor::black>() : is_in_check<PieceColor::white>();
}

bool GameBoard::is_active_in_check() const
{
    return is_color_in_check(active_color());
}

bool GameBoard::test_move_for_self_check(const BitBoardMove& move) const
{
    GameBoard test_board{*this};
    test_board.make_move(move);
    return test_board.is_color_in_check(test_board.inactive_color());
}

bool GameBoard::is_in_checkmate() const
{
    return is_color_in_check(active_color()) && !has_valid_move();
}

bool GameBoard::is_in_stalemate() const
{
    return !is_color_in_check(active_color()) && !has_valid_move();
}

GameBoard::Position GameBoard::active_king_position() const
{
    return (active_color_board() & kings()).to_position();
}

bool GameBoard::is_promotion_move(const Move move) const
{
    const auto piece = pieces_.at_checked(move.from);
    return (piece.color == PieceColor::black) ? is_promotion_move<PieceColor::black>(BitBoardMove::from_move(move))
                                              : is_promotion_move<PieceColor::white>(BitBoardMove::from_move(move));
}

[[nodiscard]] BitBoard GameBoard::valid_moves_bitboard(BitBoard from) const
{
    return (active_color() == PieceColor::black) ? valid_moves_bitboard<PieceColor::black>(from)
                                                 : valid_moves_bitboard<PieceColor::white>(from);
}

std::vector<GameBoard::Position> GameBoard::valid_moves_vector(const Position from)
{
    return valid_moves_bitboard(BitBoard{from}).to_position_vector();
}

std::set<GameBoard::Position> GameBoard::valid_moves_set(const Position from)
{
    return valid_moves_bitboard(BitBoard{from}).to_position_set();
}

PieceColor GameBoard::active_color() const
{
    return active_color_;
}

PieceColor GameBoard::inactive_color() const
{
    return opposite_color(active_color_);
}

bool GameBoard::is_active_piece(const Position& position) const
{
    return active_color_board().test(position);
}

BitBoard GameBoard::knight_moves(const BitBoard from) const
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

    assert(knights().test_all(from) && "not a knight");
    const auto offset = from.to_position() - knight_moves_origin;
    return BitBoard::shift(knight_moves_board, offset);
}

BitBoard GameBoard::bishop_moves(const BitBoard from) const
{
    static constexpr std::array<Direction, 4> directions = {
        Direction::upright, Direction::upleft, Direction::downleft, Direction::downright};
    assert(bishops().test_all(from) && "not a bishop");
    return pieces_.sliding_moves(directions, from);
}

BitBoard GameBoard::rook_moves(const BitBoard from) const
{
    static constexpr std::array<Direction, 4> directions = {
        Direction::right, Direction::up, Direction::left, Direction::down};
    assert(rooks().test_all(from) && "not a rook");
    return pieces_.sliding_moves(directions, from);
}

BitBoard GameBoard::queen_moves(const BitBoard from) const
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
    assert(queens().test_any(from) && "not a queen");
    return pieces_.sliding_moves(directions, from);
}

bool GameBoard::white_can_castle_kingside() const
{
    static constexpr BitBoard between_squares{0x00'00'00'00'00'00'00'06};
    static constexpr BitBoard king_squares{0x00'00'00'00'00'00'00'0E};
    return !white_kingside_castle_piece_moved_ && can_castle<PieceColor::white>(between_squares, king_squares);
}

bool GameBoard::white_can_castle_queenside() const
{
    static constexpr BitBoard between_squares{0x00'00'00'00'00'00'00'70};
    static constexpr BitBoard king_squares{0x00'00'00'00'00'00'00'38};
    return !white_queenside_castle_piece_moved_ && can_castle<PieceColor::white>(between_squares, king_squares);
}

bool GameBoard::black_can_castle_kingside() const
{
    static constexpr BitBoard between_squares{0x06'00'00'00'00'00'00'00};
    static constexpr BitBoard king_squares{0x0E'00'00'00'00'00'00'00};
    return !black_kingside_castle_piece_moved_ && can_castle<PieceColor::black>(between_squares, king_squares);
}

bool GameBoard::black_can_castle_queenside() const
{
    static constexpr BitBoard between_squares{0x70'00'00'00'00'00'00'00};
    static constexpr BitBoard king_squares{0x38'00'00'00'00'00'00'00};
    return !black_queenside_castle_piece_moved_ && can_castle<PieceColor::black>(between_squares, king_squares);
}

BitBoard GameBoard::king_standard_moves(const BitBoard from) const
{
    assert(kings().test_all(from) && "not a king");
    return BitBoard::neighbors_cardinal_and_diagonal(from);
}

bool GameBoard::has_valid_move() const
{
    return std::ranges::any_of(active_color_board().to_bitboard_vector(), [this](const auto from) {
        return !valid_moves_bitboard(from).empty();
    });
}

void GameBoard::set_state(const BoardState& state)
{
    en_passant_square_ = state.en_passant_square;
    pieces_ = state.pieces;
    active_color_ = state.active_color;
    black_queenside_castle_piece_moved_ = state.black_queenside_castle_piece_moved;
    black_kingside_castle_piece_moved_ = state.black_kingside_castle_piece_moved;
    white_queenside_castle_piece_moved_ = state.white_queenside_castle_piece_moved;
    white_kingside_castle_piece_moved_ = state.white_kingside_castle_piece_moved;
}

} // namespace chess
