#include "bit_board.h"

#include <bitset>
#include <cassert>
#include <set>
#include <vector>

template <>
bool BitBoard::on_edge<right>() const
{
    return (bits_ & right_edge) != 0U;
}

template <>
bool BitBoard::on_edge<upright>() const
{
    return (bits_ & top_right_edge) != 0U;
}

template <>
bool BitBoard::on_edge<up>() const
{
    return (bits_ & top_edge) != 0U;
}

template <>
bool BitBoard::on_edge<upleft>() const
{
    return (bits_ & top_left_edge) != 0U;
}

template <>
bool BitBoard::on_edge<left>() const
{
    return (bits_ & left_edge) != 0U;
}

template <>
bool BitBoard::on_edge<downright>() const
{
    return (bits_ & bottom_right_edge) != 0U;
}

template <>
bool BitBoard::on_edge<down>() const
{
    return (bits_ & bottom_edge) != 0U;
}

template <>
bool BitBoard::on_edge<downleft>() const
{
    return (bits_ & bottom_left_edge) != 0U;
}

bool BitBoard::on_edge(const Direction direction) const
{
    switch (direction) {
    case right:
        return on_edge<right>();
    case upright:
        return on_edge<upright>();
    case up:
        return on_edge<up>();
    case upleft:
        return on_edge<upleft>();
    case left:
        return on_edge<left>();
    case downleft:
        return on_edge<downleft>();
    case down:
        return on_edge<down>();
    case downright:
        return on_edge<downright>();
    }
    assert(!"invalid direction");
    return {};
}

bool BitBoard::on_any_edge() const
{
    return test_any(all_edge);
}

BitBoard BitBoard::shift(BitBoard board, const Direction direction, const size_t n)
{
    return board.shift_assign(direction, n);
}
/*
BitBoard BitBoard::shift(const Direction direction, const size_t n) const
{
    BitBoard shifted{*this};
    shifted.shift_assign(direction, n);
    return shifted;
}
*/

BitBoard& BitBoard::dilate(const Direction direction, const size_t n)
{
    switch (direction) {
    case right:
        return dilate<right>(n);
    case upright:
        return dilate<upright>(n);
    case up:
        return dilate<up>(n);
    case upleft:
        return dilate<upleft>(n);
    case left:
        return dilate<left>(n);
    case downleft:
        return dilate<downleft>(n);
    case down:
        return dilate<down>(n);
    case downright:
        return dilate<downright>(n);
    }
    assert(!"invalid direction");
    return *this;
}

template <>
BitBoard& BitBoard::shift_assign<Direction::up>(const size_t n)
{
    bits_ <<= (board_size * n);
    return *this;
}
template <>
BitBoard& BitBoard::shift_assign<Direction::down>(const size_t n)
{
    bits_ >>= (board_size * n);
    return *this;
}
template <>
BitBoard& BitBoard::shift_assign<Direction::left>(const size_t n)
{
    Bits wall{0};
    for (size_t i = 0; i < n; i++) {
        wall |= (right_edge << i);
    }
    bits_ <<= n;
    bits_ &= ~wall;
    return *this;
}
template <>
BitBoard& BitBoard::shift_assign<Direction::right>(const size_t n)
{
    Bits wall{0};
    for (size_t i = 0; i < n; i++) {
        wall |= (left_edge >> i);
    }
    bits_ >>= n;
    bits_ &= ~wall;
    return *this;
}
template <>
BitBoard& BitBoard::shift_assign<Direction::upright>(const size_t n)
{
    return shift_assign<Direction::up>(n).shift_assign<Direction::right>(n);
}
template <>
BitBoard& BitBoard::shift_assign<Direction::upleft>(const size_t n)
{
    return shift_assign<Direction::up>(n).shift_assign<Direction::left>(n);
}
template <>
BitBoard& BitBoard::shift_assign<Direction::downright>(const size_t n)
{
    return shift_assign<Direction::down>(n).shift_assign<Direction::right>(n);
}
template <>
BitBoard& BitBoard::shift_assign<Direction::downleft>(const size_t n)
{
    return shift_assign<Direction::down>(n).shift_assign<Direction::left>(n);
}

BitBoard& BitBoard::shift_assign(const Direction direction, const size_t n)
{
    switch (direction) {
    case right:
        return shift_assign<right>(n);
    case upright:
        return shift_assign<upright>(n);
    case up:
        return shift_assign<up>(n);
    case upleft:
        return shift_assign<upleft>(n);
    case left:
        return shift_assign<left>(n);
    case downleft:
        return shift_assign<downleft>(n);
    case down:
        return shift_assign<down>(n);
    case downright:
        return shift_assign<downright>(n);
    }
    assert(!"invalid direction");
    return *this;
}

BitBoard& BitBoard::shift_assign(const BitBoard::Position relative_offset)
{
    if (relative_offset.x() >= 0) {
        shift_assign<Direction::down>(relative_offset.x());
    } else {
        shift_assign<Direction::up>(-relative_offset.x());
    }
    if (relative_offset.y() >= 0) {
        shift_assign<Direction::right>(relative_offset.y());
    } else {
        shift_assign<Direction::left>(-relative_offset.y());
    }
    return *this;
}

BitBoard BitBoard::from_position(const Position& position)
{
    return BitBoard::make_top_left().shift_assign<down>(position.x()).shift_assign<right>(position.y());
}

BitBoard BitBoard::neighbors_cardinal(const Position& position)
{
    auto board = BitBoard{position};
    return shift<right>(board) | shift<up>(board) | shift<left>(board) | shift<down>(board);
}

BitBoard BitBoard::neighbors_diagonal(const Position& position)
{
    auto board = BitBoard{position};
    return shift<upright>(board) | shift<upleft>(board) | shift<downleft>(board) | shift<downright>(board);
}

BitBoard BitBoard::neighbors_cardinal_and_diagonal(const Position& position)
{
    return neighbors_cardinal(position) | neighbors_diagonal(position);
}

std::vector<BitBoard::Position> BitBoard::to_position_vector() const
{
    std::vector<Position> positions;
    for (int column = 0; column < board_size; column++) {
        for (int row = 0; row < board_size; row++) {
            Position position{row, column};
            if (test(position)) {
                positions.push_back(position);
            }
        }
    }
    return positions;
}

std::set<BitBoard::Position> BitBoard::to_position_set() const
{
    std::set<Position> positions;
    for (int column = 0; column < board_size; column++) {
        for (int row = 0; row < board_size; row++) {
            Position position{row, column};
            if (test(position)) {
                positions.insert(position);
            }
        }
    }
    return positions;
}

std::bitset<BitBoard::board_size * BitBoard::board_size> BitBoard::to_bitset() const
{
    return std::bitset<board_size * board_size>{bits_};
}
