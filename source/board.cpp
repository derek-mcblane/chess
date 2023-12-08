#include "board.h"

#include "vec2.h"

#include <cassert>

#include <algorithm>
#include <map>
#include <optional>
#include <set>

namespace chess {

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
    for (Position::dimension_type column = 0; column < BitBoard::board_size; ++column) {
        pieces.set({PieceColor::black, back_row[column]}, {black_piece_row_index, column});
        pieces.set({PieceColor::black, PieceType::pawn}, {black_pawn_row_index, column});
        pieces.set({PieceColor::white, PieceType::pawn}, {white_pawn_row_index, column});
        pieces.set({PieceColor::white, back_row[column]}, {white_piece_row_index, column});
    }
    return pieces;
}

std::optional<Piece> BoardPieces::at(const BitBoard position) const noexcept
{
    const auto piece_color = color_at(position);
    const auto piece_type = type_at(position);
    if (!piece_color.has_value() || !piece_type.has_value()) {
        return {};
    }
    assert(piece_color.has_value() && piece_type.has_value() && "inconsistent bitboards");
    return Piece{.color = *piece_color, .type = *piece_type};
}

std::optional<Piece> BoardPieces::at(const Position& position) const
{
    return at(BitBoard{position});
}

Piece BoardPieces::at_checked(const BitBoard position) const
{
    const auto piece = at(position);
    if (!piece.has_value()) {
        throw std::runtime_error("no piece at position");
    }
    return *piece;
}

Piece BoardPieces::at_checked(const Position& position) const
{
    return at_checked(BitBoard{position});
}

std::optional<PieceColor> BoardPieces::color_at(const BitBoard position) const noexcept
{
    assert(position.has_single_position());
    return (black_.test_all(position))   ? std::optional<PieceColor>{PieceColor::black}
           : (white_.test_all(position)) ? std::optional<PieceColor>{PieceColor::white}
                                         : std::nullopt;
}

std::optional<PieceColor> BoardPieces::color_at(const Position& position) const
{
    return color_at(BitBoard{position});
}

std::optional<PieceType> BoardPieces::type_at(const BitBoard position) const noexcept
{
    assert(position.has_single_position());
    return (pawns_.test_all(position))     ? std::optional<PieceType>{PieceType::pawn}
           : (knights_.test_all(position)) ? std::optional<PieceType>{PieceType::knight}
           : (bishops_.test_all(position)) ? std::optional<PieceType>{PieceType::bishop}
           : (rooks_.test_all(position))   ? std::optional<PieceType>{PieceType::rook}
           : (queens_.test_all(position))  ? std::optional<PieceType>{PieceType::queen}
           : (kings_.test_all(position))   ? std::optional<PieceType>{PieceType::king}
                                           : std::nullopt;
}

std::optional<PieceType> BoardPieces::type_at(const Position& position) const
{
    return type_at(BitBoard{position});
}

BitBoard BoardPieces::sliding_moves(const Direction direction, const BitBoard from, const size_t range) const
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

} // namespace chess
