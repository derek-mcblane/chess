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
    return (bits_ & (top_edge | right_edge)) != 0U;
}

template <>
bool BitBoard::on_edge<up>() const
{
    return (bits_ & top_edge) != 0U;
}

template <>
bool BitBoard::on_edge<upleft>() const
{
    return (bits_ & (top_edge | left_edge)) != 0U;
}

template <>
bool BitBoard::on_edge<left>() const
{
    return (bits_ & left_edge) != 0U;
}

template <>
bool BitBoard::on_edge<downright>() const
{
    return (bits_ & (bottom_edge | right_edge)) != 0U;
}

template <>
bool BitBoard::on_edge<down>() const
{
    return (bits_ & bottom_edge) != 0U;
}

template <>
bool BitBoard::on_edge<downleft>() const
{
    return (bits_ & (bottom_edge | left_edge)) != 0U;
}

template <>
BitBoard BitBoard::shift<up>(const size_t n) const
{
    return bits_ << (board_size * n);
}

template <>
BitBoard BitBoard::shift<down>(const size_t n) const
{
    return bits_ >> (board_size * n);
}

template <>
BitBoard BitBoard::shift<right>(const size_t n) const
{
    BitBoard shifted{bits_};
    Bits wall{0};
    for (size_t i = 0; i < n; i++) {
        wall |= (left_edge >> i);
    }
    shifted >>= n;
    shifted &= ~wall;
    return shifted;
}

template <>
BitBoard BitBoard::shift<left>(const size_t n) const
{
    BitBoard shifted{bits_};
    Bits wall{0};
    for (size_t i = 0; i < n; i++) {
        wall |= (right_edge << i);
    }
    shifted <<= n;
    shifted &= ~wall;
    return shifted;
}

template <>
BitBoard BitBoard::shift<upright>(const size_t n) const
{
    return shift<up>(n).shift<right>(n);
}

template <>
BitBoard BitBoard::shift<upleft>(const size_t n) const
{
    return shift<up>(n).shift<left>(n);
}

template <>
BitBoard BitBoard::shift<downleft>(const size_t n) const
{
    return shift<down>(n).shift<left>(n);
}

template <>
BitBoard BitBoard::shift<downright>(const size_t n) const
{
    return shift<down>(n).shift<right>(n);
}

BitBoard BitBoard::shift(const Direction direction, const size_t n) const
{
    switch (direction) {
    case right:
        return shift<right>(n);
    case upright:
        return shift<upright>(n);
    case up:
        return shift<up>(n);
    case upleft:
        return shift<upleft>(n);
    case left:
        return shift<left>(n);
    case downleft:
        return shift<downleft>(n);
    case down:
        return shift<down>(n);
    case downright:
        return shift<downright>(n);
    }
    assert(!"invalid direction");
    return {};
}

BitBoard BitBoard::neighbors_cardinal(const Position& position)
{
    BitBoard board{position_mask(position)};
    return board.shift<right>() | board.shift<up>() | board.shift<left>() | board.shift<down>();
}

BitBoard BitBoard::neighbors_diagonal(const Position& position)
{
    BitBoard board{position_mask(position)};
    return board.shift<upright>() | board.shift<upleft>() | board.shift<downleft>() | board.shift<downright>();
}

BitBoard BitBoard::neighbors_cardinal_and_diagonal(const Position& position)
{
    return neighbors_cardinal(position) | neighbors_diagonal(position);
}

std::vector<BitBoard::Position> BitBoard::to_position_vector() const
{
    std::vector<Position> positions;
    for (int row = 0; row < board_size; row++) {
        for (int column = 0; column < board_size; column++) {
            Position position{row, column};
            if ((position_mask(position) & bits_) != 0U) {
                positions.push_back(position);
            }
        }
    }
    return positions;
}

std::set<BitBoard::Position> BitBoard::to_position_set() const
{
    std::set<Position> positions;
    for (int row = 0; row < board_size; row++) {
        for (int column = 0; column < board_size; column++) {
            Position position{row, column};
            if ((position_mask(position) & bits_) != 0U) {
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
