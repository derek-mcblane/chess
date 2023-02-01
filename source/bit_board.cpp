#include "bit_board.h"

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
BitBoard BitBoard::shift<up>(const size_t &n) const
{
    return bits_ << (board_size * n);
}
template <>
BitBoard BitBoard::shift<down>(const size_t &n) const
{
    return bits_ >> (board_size * n);
}
template <>
BitBoard BitBoard::shift<right>(const size_t &n) const
{
    BitBoard shifted{bits_};
    BitsT wall{0};
    for (int i = 0; i < n; i++) {
        wall |= (left_edge >> i);
    }
    wall = ~wall;
    shifted >>= (1 * n);
    shifted &= wall;
    return shifted;
}
template <>
BitBoard BitBoard::shift<left>(const size_t &n) const
{
    BitBoard shifted{bits_};
    BitsT wall{0};
    for (int i = 0; i < n; i++) {
        wall |= (right_edge << i);
    }
    wall = ~wall;
    shifted <<= (1 * n);
    shifted &= wall;
    return shifted;
}
template <>
BitBoard BitBoard::shift<upright>(const size_t &n) const
{
    return shift<up>(n).shift<right>(n);
}
template <>
BitBoard BitBoard::shift<upleft>(const size_t &n) const
{
    return shift<up>(n).shift<left>(n);
}
template <>
BitBoard BitBoard::shift<downleft>(const size_t &n) const
{
    return shift<down>(n).shift<left>(n);
}
template <>
BitBoard BitBoard::shift<downright>(const size_t &n) const
{
    return shift<down>(n).shift<right>(n);
}

std::vector<BitBoard::Position> BitBoard::to_positions() const
{
    std::vector<Position> positions;
    for (size_t row = 0; row < board_size; row++) {
        for (size_t col = 0; col < board_size; col++) {
            Position p{row, col};
            if (position_mask(p) & bits_)
                positions.push_back(p);
        }
    }
    return positions;
}

std::bitset<BitBoard::board_size * BitBoard::board_size> BitBoard::to_bitset() const
{
    return std::bitset<board_size * board_size>{bits_};
}
