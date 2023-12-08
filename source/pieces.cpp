#include "pieces.h"

#include "bit_board.h"

#include <map>

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

const std::map<PieceColor, std::string> piece_color_short_names{
    {PieceColor::white, "W"},
    {PieceColor::black, "B"},
};

const std::map<PieceType, std::string> piece_type_short_names{
    {PieceType::pawn, "p"},
    {PieceType::knight, "n"},
    {PieceType::bishop, "b"},
    {PieceType::rook, "r"},
    {PieceType::queen, "q"},
    {PieceType::king, "k"},
};

} // namespace chess
