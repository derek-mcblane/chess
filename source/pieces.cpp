#include "pieces.h"

namespace chess {

const std::map<PieceColor, std::string> piece_color_names{
    {PieceColor::white, "white"},
    {PieceColor::black, "black"},
    {PieceColor::none, "none"},
};

const std::map<PieceType, std::string> piece_type_names{
    {PieceType::pawn, "pawn"}, {PieceType::knight, "knight"}, {PieceType::bishop, "bishop"},
    {PieceType::rook, "rook"}, {PieceType::queen, "queen"},   {PieceType::king, "king"},
    {PieceType::none, "none"},
};

void Pieces::set_piece(const Piece& piece, const Coord& position)
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

void Pieces::clear_piece(const Coord& position)
{
    if (!occupied_.test(position)) {
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

[[nodiscard]] bool Pieces::occupied(const Coord& position) const
{
    return occupied_.test(position);
}

[[nodiscard]] Piece Pieces::occupant_at(const Coord& position) const
{
    if (!occupied(position)) {
        return null_piece;
    }
    return Piece{.color = piece_color_at(position), .type = piece_type_at(position)};
}

[[nodiscard]] PieceColor Pieces::piece_color_at(const Coord& position) const
{
    if (!occupied_.test(position)) {
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

[[nodiscard]] PieceType Pieces::piece_type_at(const Coord& position) const
{
    if (!occupied_.test(position)) {
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

} // namespace chess
