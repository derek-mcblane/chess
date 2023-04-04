#pragma once

#include "vec2.h"

#include <bitset>
#include <cassert>
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
    using Position = dm::Vec2<int>;
    using Bits = std::uint64_t;

    static constexpr size_t board_size = 8;
    static constexpr size_t n_bits = 64;
    static_assert(sizeof(Bits) * CHAR_BIT == n_bits);

    constexpr BitBoard() : BitBoard(0) {}
    constexpr BitBoard(const Bits bits) : bits_(bits) {}
    constexpr BitBoard(const std::bitset<n_bits> bits) : bits_(bits.to_ullong()) {}
    constexpr BitBoard(const Position& position) : BitBoard(from_position(position)) {}

    constexpr BitBoard(const BitBoard& other) = default;
    constexpr BitBoard& operator=(const BitBoard&) = default;

    constexpr BitBoard(BitBoard&& other) = default;
    constexpr BitBoard& operator=(BitBoard&&) = default;

    static BitBoard make_top_right()
    {
        return top_right;
    }

    static BitBoard make_top_left()
    {
        return top_left;
    }

    static BitBoard make_bottom_left()
    {
        return bottom_left;
    }

    static BitBoard make_bottom_right()
    {
        return bottom_right;
    }

    static BitBoard make_right_edge()
    {
        return right_edge;
    }

    static BitBoard make_top_right_edge()
    {
        return top_right_edge;
    }

    static BitBoard make_top_edge()
    {
        return top_edge;
    }

    static BitBoard make_top_left_edge()
    {
        return top_left_edge;
    }

    static BitBoard make_left_edge()
    {
        return left_edge;
    }

    static BitBoard make_bottom_left_edge()
    {
        return bottom_left_edge;
    }

    static BitBoard make_bottom_edge()
    {
        return bottom_edge;
    }

    static BitBoard make_bottom_right_edge()
    {
        return bottom_right_edge;
    }

    static BitBoard make_positive_slope()
    {
        return positive_slope;
    }

    static BitBoard make_negative_slope()
    {
        return negative_slope;
    }

    static BitBoard make_full()
    {
        return ~BitBoard{};
    }

    template <Direction D>
    [[nodiscard]] static BitBoard shift(BitBoard board, size_t n = 1)
    {
        return board.shift_assign<D>(n);
    }

    [[nodiscard]] static BitBoard shift(BitBoard board, Direction direction, size_t n = 1);

    static BitBoard neighbors_cardinal(const Position& position);
    static BitBoard neighbors_diagonal(const Position& position);
    static BitBoard neighbors_cardinal_and_diagonal(const Position& position);

    [[nodiscard]] bool test(const Position& position) const
    {
        return test_any(BitBoard{position});
    }

    [[nodiscard]] bool test_any(const BitBoard& other) const
    {
        return !(*this & other).empty();
    }

    [[nodiscard]] bool test_all(const BitBoard& other) const
    {
        return (*this & other) == other;
    }

    [[nodiscard]] bool empty() const
    {
        return bits_ == 0U;
    }

    [[nodiscard]] std::size_t count() const
    {
        return std::bitset<n_bits>(bits_).count();
    }

    BitBoard& set(const BitBoard& other)
    {
        bits_ |= other.bits_;
        return *this;
    }

    BitBoard& set(const Position& position)
    {
        return set(BitBoard{position});
    }

    BitBoard& reset(const BitBoard& other)
    {
        *this &= ~other;
        return *this;
    }

    BitBoard& reset(const Position& position)
    {
        return reset(BitBoard{position});
    }

    BitBoard& reset_all()
    {
        return reset(BitBoard::make_full());
    }

    template <Direction D>
    [[nodiscard]] bool on_edge() const;

    [[nodiscard]] bool on_edge(Direction direction) const;

    [[nodiscard]] bool on_any_edge() const;

    template <Direction D>
    BitBoard& shift_assign(size_t n = 1);

    BitBoard& shift_assign(Direction direction, size_t n = 1);

    BitBoard& shift_assign(Position relative_offset);

    /*
    template <Direction D>
    [[nodiscard]] BitBoard shift(size_t n = 1) const
    {
        BitBoard shifted{*this};
        shifted.shift_assign<D>(n);
        return shifted;
    }

    [[nodiscard]] BitBoard shift(Direction direction, size_t n = 1) const;
    */

    template <Direction D>
    BitBoard& dilate(const size_t n = 1)
    {
        for (size_t i = 0; i < n; i++) {
            *this |= BitBoard::shift<D>(*this);
        }
        return *this;
    }

    BitBoard& dilate(Direction direction, size_t n = 1);

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
        return !(*this == other);
    }
    BitBoard& operator<<=(size_t n)
    {
        bits_ <<= n;
        return *this;
    }
    BitBoard operator<<(size_t n) const
    {
        return BitBoard{*this} <<= n;
    }
    BitBoard& operator>>=(size_t n)
    {
        bits_ >>= n;
        return *this;
    }
    BitBoard operator>>(size_t n) const
    {
        return BitBoard{*this} >>= n;
    }
    BitBoard& operator|=(const BitBoard& other)
    {
        bits_ |= other.bits_;
        return *this;
    }
    BitBoard operator|(const BitBoard& other) const
    {
        return BitBoard{*this} |= other;
    }
    BitBoard& operator&=(const BitBoard& other)
    {
        bits_ &= other.bits_;
        return *this;
    }
    BitBoard operator&(const BitBoard& other) const
    {
        return BitBoard{*this} &= other;
    }
    BitBoard& operator^=(const BitBoard& other)
    {
        bits_ ^= other.bits_;
        return *this;
    }
    BitBoard operator^(const BitBoard& other) const
    {
        return BitBoard{bits_ ^ other.bits_};
    }
    BitBoard operator~() const
    {
        return BitBoard{~bits_};
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
    static constexpr Bits top_right_edge = top_edge | right_edge;
    static constexpr Bits top_left_edge = top_edge | left_edge;
    static constexpr Bits bottom_right_edge = bottom_edge | right_edge;
    static constexpr Bits bottom_left_edge = bottom_edge | left_edge;
    static constexpr Bits all_edge = right_edge | top_edge | left_edge | bottom_edge;
    static constexpr Bits negative_slope = 0b10000000'01000000'00100000'00010000'00001000'00000100'00000010'00000001;
    static constexpr Bits positive_slope = 0b00000001'00000010'00000100'00001000'00010000'00100000'01000000'10000000;

    static BitBoard from_position(const Position& position);
};
