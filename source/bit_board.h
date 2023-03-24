#pragma once

#include "vec2.h"

#include <bitset>
#include <set>
#include <vector>

enum Direction
{
    right,
    upright,
    up,
    upleft,
    left,
    downleft,
    down,
    downright,
};

class BitBoard
{
  public:
    using Position = dm::Coord<int>;
    using Bits = std::uint64_t;

    static constexpr size_t board_size = 8;
    static constexpr size_t n_bits = 64;
    static_assert(sizeof(Bits) * CHAR_BIT == n_bits);

    constexpr BitBoard() : BitBoard(0) {}
    constexpr BitBoard(const Bits bits) : bits_(bits) {}
    constexpr BitBoard(const std::bitset<n_bits> bits) : bits_(bits.to_ullong()) {}

    constexpr BitBoard(const BitBoard& other) = default;
    constexpr BitBoard& operator=(const BitBoard&) = default;

    constexpr BitBoard(BitBoard&& other) = default;
    constexpr BitBoard& operator=(BitBoard&&) = default;

    static Bits position_mask(const Position& position)
    {
        return (top_left >> (position.row() * board_size) >> (position.column()));
    }

    static BitBoard neighbors_cardinal(const Position& position);
    static BitBoard neighbors_diagonal(const Position& position);
    static BitBoard neighbors_cardinal_and_diagonal(const Position& position);

    [[nodiscard]] bool test(const Position& position) const
    {
        return (bits_ & position_mask(position)) != 0U;
    }

    [[nodiscard]] bool empty() const
    {
        return bits_ == 0U;
    }

    [[nodiscard]] std::size_t count() const
    {
        return std::bitset<n_bits>(bits_).count();
    }

    void set(const Position& position)
    {
        bits_ |= position_mask(position);
    }

    void reset(const Position& position)
    {
        bits_ &= ~position_mask(position);
    }

    template <Direction>
    [[nodiscard]] bool on_edge() const;

    template <Direction D>
    [[nodiscard]] BitBoard shift(const size_t n = 1) const;

    [[nodiscard]] BitBoard shift(Direction direction, const size_t n = 1) const;

    template <Direction D>
    BitBoard dilate(const size_t n = 1)
    {
        BitBoard dilated{bits_};
        for (size_t i = 0; i < n; i++) {
            dilated |= dilated.shift<D>(n);
        }
        return dilated;
    }

    [[nodiscard]] std::vector<Position> to_position_vector() const;
    [[nodiscard]] std::set<Position> to_position_set() const;
    [[nodiscard]] std::bitset<n_bits> to_bitset() const;

    [[nodiscard]] std::string to_string() const
    {
        return std::bitset<n_bits>(bits_).to_string();
    }

    bool operator==(const BitBoard& other) const
    {
        return bits_ == other.bits_;
    }
    bool operator!=(const BitBoard& other) const
    {
        return bits_ != other.bits_;
    }
    BitBoard& operator<<=(size_t n)
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
    BitBoard& operator>>=(size_t n)
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
    BitBoard operator|(const BitBoard& other) const
    {
        BitBoard result{*this};
        result |= other;
        return result;
    }
    BitBoard& operator|=(const BitBoard& other)
    {
        bits_ |= other.bits_;
        return *this;
    }
    BitBoard operator&(const BitBoard& other) const
    {
        BitBoard result{bits_};
        result &= other;
        return result;
    }
    BitBoard& operator&=(const BitBoard& other)
    {
        bits_ &= other.bits_;
        return *this;
    }
    BitBoard operator^(const BitBoard& other) const
    {
        return bits_ ^ other.bits_;
    }
    BitBoard operator~() const
    {
        return ~bits_;
    }

  private:
    Bits bits_;

    static constexpr Bits top_right = 0b00000001'00000000'00000000'00000000'00000000'00000000'00000000'00000000;
    static constexpr Bits top_left = 0b10000000'00000000'00000000'00000000'00000000'00000000'00000000'00000000;
    static constexpr Bits bottom_left = 0b00000000'00000000'00000000'00000000'00000000'00000000'00000000'10000000;
    static constexpr Bits bottom_right = 0b00000000'00000000'00000000'00000000'00000000'00000000'00000000'00000001;
    static constexpr Bits top_edge = 0b11111111'00000000'00000000'00000000'00000000'00000000'00000000'00000000;
    static constexpr Bits bottom_edge = 0b00000000'00000000'00000000'00000000'00000000'00000000'00000000'11111111;
    static constexpr Bits left_edge = 0b10000000'10000000'10000000'10000000'10000000'10000000'10000000'10000000;
    static constexpr Bits right_edge = 0b00000001'00000001'00000001'00000001'00000001'00000001'00000001'00000001;
    static constexpr Bits neg_slope = 0b10000000'01000000'00100000'00010000'00001000'00000100'00000010'00000001;
    static constexpr Bits pos_slope = 0b00000001'00000010'00000100'00001000'00010000'00100000'01000000'10000000;
};
