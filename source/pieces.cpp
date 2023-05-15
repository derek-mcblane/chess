#include "pieces.h"

#include "bit_board.h"

#include "bit_board_formatter.h"
#include "piece_formatter.h"
#include "vec2_formatter.h"

#include <spdlog/spdlog.h>

#include <cassert>
#include <exception>
#include <map>
#include <optional>
#include <set>
#include <utility>

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

} // namespace chess
