#pragma once

#include <array>
#include <optional>
#include <vector>

#include "vec2.h"

namespace chess {

enum class Piece
{
    pawn,
    knight,
    bishop,
    rook,
    queen,
    king,
};

class Board
{
    using Position = dm::Vec2<int>;

  public:
    std::vector<Position> valid_moves(Position position);
    void move(Position from_position, Position to_position);

  private:
    // piece locations, did king move, did rooks move, what was the last pawn that moved
    static constexpr std::size_t board_size = 8;
    std::array<std::array<Piece, board_size>, board_size> board_;
    bool king_moved_ = false;
    std::optional<Position> last_moved_pawn_;
};

} // namespace chess
