#pragma once

#include "vec2.h"

#include <bitset>
#include <vector>

enum Direction
{
    min_direction_ = 0,
    right = min_direction_,
    upright,
    up,
    upleft,
    left,
    downleft,
    down,
    downright,
    max_direction_,
};

class BitBoard
{
  public:
    using Position = dm::Coord<int>;
    using BitsT = std::uint64_t;

    static constexpr size_t board_size = 8;

    BitBoard() : BitBoard(0) {}
    BitBoard(const BitsT bits) : bits_(bits) {}

    BitBoard(const BitBoard &other) = default;
    BitBoard &operator=(const BitBoard &) = default;

    BitBoard(BitBoard &&other) = default;
    BitBoard &operator=(BitBoard &&) = default;

    static BitsT position_mask(const Position &position)
    {
        return (top_left >> (position.row() * board_size) >> (position.column()));
    }

    [[nodiscard]] bool test(const Position &position) const
    {
        return (bits_ & position_mask(position)) != 0U;
    }

    [[nodiscard]] bool count() const
    {
        return std::bitset<board_size * board_size>(bits_).count() != 0U;
    }

    void set(const Position &position)
    {
        bits_ |= position_mask(position);
    }

    void reset(const Position &position)
    {
        bits_ &= ~position_mask(position);
    }

    template <Direction>
    [[nodiscard]] bool on_edge() const;

    template <Direction D>
    [[nodiscard]] BitBoard shift(const size_t &n = 1) const;

    template <Direction D>
    BitBoard dilate(const size_t &n = 1)
    {
        BitBoard dilated{bits_};
        for (int i = 0; i < n; i++) {
            dilated |= dilated.shift<D>(n);
        }
        return dilated;
    }

    [[nodiscard]] std::vector<Position> to_positions() const;
    [[nodiscard]] std::bitset<board_size * board_size> to_bitset() const;

    [[nodiscard]] std::string to_string() const
    {
        return std::bitset<board_size * board_size>(bits_).to_string();
    }

    bool operator==(const BitBoard &other) const
    {
        return bits_ == other.bits_;
    }
    bool operator!=(const BitBoard &other) const
    {
        return bits_ != other.bits_;
    }
    BitBoard &operator<<=(size_t n)
    {
        bits_ <<= n;
        return *this;
    }
    BitBoard operator<<(size_t n) const
    {
        BitBoard result{bits_};
        result.bits_ <<= n;
        return result;
    }
    BitBoard &operator>>=(size_t n)
    {
        bits_ >>= n;
        return *this;
    }
    BitBoard operator>>(size_t n) const
    {
        BitBoard result{bits_};
        result.bits_ >>= n;
        return result;
    }
    BitBoard operator|(const BitBoard &other) const
    {
        BitBoard result{bits_};
        result |= other;
        return result;
    }
    BitBoard &operator|=(const BitBoard &other)
    {
        bits_ |= other.bits_;
        return *this;
    }
    BitBoard operator&(const BitBoard &other) const
    {
        BitBoard result{bits_};
        result &= other;
        return result;
    }
    BitBoard &operator&=(const BitBoard &other)
    {
        bits_ &= other.bits_;
        return *this;
    }
    BitBoard operator^(const BitBoard &other) const
    {
        return bits_ ^ other.bits_;
    }
    BitBoard operator~() const
    {
        return ~bits_;
    }
    operator bool() const
    {
        return bits_ != 0U;
    }

  private:
    BitsT bits_;

    static constexpr BitsT top_right =
        0b00000001'00000000'00000000'00000000'00000000'00000000'00000000'00000000;
    static constexpr BitsT top_left =
        0b10000000'00000000'00000000'00000000'00000000'00000000'00000000'00000000;
    static constexpr BitsT bottom_left =
        0b00000000'00000000'00000000'00000000'00000000'00000000'00000000'10000000;
    static constexpr BitsT bottom_right =
        0b00000000'00000000'00000000'00000000'00000000'00000000'00000000'00000001;
    static constexpr BitsT top_edge =
        0b11111111'00000000'00000000'00000000'00000000'00000000'00000000'00000000;
    static constexpr BitsT bottom_edge =
        0b00000000'00000000'00000000'00000000'00000000'00000000'00000000'11111111;
    static constexpr BitsT left_edge =
        0b10000000'10000000'10000000'10000000'10000000'10000000'10000000'10000000;
    static constexpr BitsT right_edge =
        0b00000001'00000001'00000001'00000001'00000001'00000001'00000001'00000001;
    static constexpr BitsT neg_slope =
        0b10000000'01000000'00100000'00010000'00001000'00000100'00000010'00000001;
    static constexpr BitsT pos_slope =
        0b00000001'00000010'00000100'00001000'00010000'00100000'01000000'10000000;
};
