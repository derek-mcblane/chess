#include "gtest/gtest.h"

#include "bit_board.h"

#include <algorithm>
#include <bitset>
#include <iterator>

static const BitBoard test_board{std::bitset<64>{"10101010"
                                                 "01010101"
                                                 "10101010"
                                                 "01010101"
                                                 "10101010"
                                                 "01010101"
                                                 "10101010"
                                                 "01010101"}};

static const BitBoard down_board{std::bitset<64>{"00000000"
                                                 "10101010"
                                                 "01010101"
                                                 "10101010"
                                                 "01010101"
                                                 "10101010"
                                                 "01010101"
                                                 "10101010"}};

static const BitBoard up_board{std::bitset<64>{"01010101"
                                               "10101010"
                                               "01010101"
                                               "10101010"
                                               "01010101"
                                               "10101010"
                                               "01010101"
                                               "00000000"}};

static const BitBoard left_board{std::bitset<64>{"01010100"
                                                 "10101010"
                                                 "01010100"
                                                 "10101010"
                                                 "01010100"
                                                 "10101010"
                                                 "01010100"
                                                 "10101010"}};

static const BitBoard right_board{std::bitset<64>{"01010101"
                                                  "00101010"
                                                  "01010101"
                                                  "00101010"
                                                  "01010101"
                                                  "00101010"
                                                  "01010101"
                                                  "00101010"}};

static const BitBoard upleft_board{std::bitset<64>{"10101010"
                                                   "01010100"
                                                   "10101010"
                                                   "01010100"
                                                   "10101010"
                                                   "01010100"
                                                   "10101010"
                                                   "00000000"}};

static const BitBoard upright_board{std::bitset<64>{"00101010"
                                                    "01010101"
                                                    "00101010"
                                                    "01010101"
                                                    "00101010"
                                                    "01010101"
                                                    "00101010"
                                                    "00000000"}};

static const BitBoard downleft_board{std::bitset<64>{"00000000"
                                                     "01010100"
                                                     "10101010"
                                                     "01010100"
                                                     "10101010"
                                                     "01010100"
                                                     "10101010"
                                                     "01010100"}};

static const BitBoard downright_board{std::bitset<64>{"00000000"
                                                      "01010101"
                                                      "00101010"
                                                      "01010101"
                                                      "00101010"
                                                      "01010101"
                                                      "00101010"
                                                      "01010101"}};

TEST(Board, SetAndTest)
{
    BitBoard bits;
    for (int row = 0; row < BitBoard::board_size; row += 1) {
        for (int col = (row % 2 == 0) ? 0 : 1; col < BitBoard::board_size; col += 2) {
            bits.set({row, col});
        }
    }
    EXPECT_EQ(bits.to_string(), test_board.to_string());
}

TEST(BoardStaticShift, ShiftRight)
{
    EXPECT_EQ(test_board.shift<Direction::right>(1).to_string(), right_board.to_string());
}

TEST(BoardStaticShift, ShiftUpRight)
{
    EXPECT_EQ(test_board.shift<Direction::upright>(1).to_string(), upright_board.to_string());
}

TEST(BoardStaticShift, ShiftUp)
{
    EXPECT_EQ(test_board.shift<Direction::up>(1).to_string(), up_board.to_string());
}

TEST(BoardStaticShift, ShiftUpLeft)
{
    EXPECT_EQ(test_board.shift<Direction::upleft>(1).to_string(), upleft_board.to_string());
}

TEST(BoardStaticShift, ShiftLeft)
{
    EXPECT_EQ(test_board.shift<Direction::left>(1).to_string(), left_board.to_string());
}

TEST(BoardStaticShift, ShiftDownLeft)
{
    EXPECT_EQ(test_board.shift<Direction::downleft>(1).to_string(), downleft_board.to_string());
}

TEST(BoardStaticShift, ShiftDown)
{
    EXPECT_EQ(test_board.shift<Direction::down>(1).to_string(), down_board.to_string());
}

TEST(BoardStaticShift, ShiftDownRight)
{
    EXPECT_EQ(test_board.shift<Direction::downright>(1).to_string(), downright_board.to_string());
}

TEST(BoardDynamicShift, ShiftAll)
{
    static const std::map<Direction, BitBoard> shifted_board_bits = {
        {Direction::right, right_board},
        {Direction::upright, upright_board},
        {Direction::up, up_board},
        {Direction::upleft, upleft_board},
        {Direction::left, left_board},
        {Direction::downleft, downleft_board},
        {Direction::down, down_board},
        {Direction::downright, downright_board},
    };

    for (const auto [direction, expected] : shifted_board_bits) {
        BitBoard shifted{test_board.shift(direction, 1)};
        EXPECT_EQ(shifted.to_string(), expected.to_string());
    }
}

TEST(BoardCardinalNeighbors, Middle)
{
    const auto neighbors = BitBoard::neighbors_cardinal({4, 4});

    EXPECT_TRUE(neighbors.test({3, 4}));
    EXPECT_TRUE(neighbors.test({4, 3}));
    EXPECT_TRUE(neighbors.test({5, 4}));
    EXPECT_TRUE(neighbors.test({4, 5}));
    EXPECT_EQ(neighbors.count(), 4);
}

TEST(BoardCardinalNeighbors, Corner)
{
    const auto neighbors = BitBoard::neighbors_cardinal({0, 7});

    EXPECT_TRUE(neighbors.test({0, 6}));
    EXPECT_TRUE(neighbors.test({1, 7}));
    EXPECT_EQ(neighbors.count(), 2);
}

TEST(BoardDiagonalNeighbors, Corner)
{
    const auto neighbors = BitBoard::neighbors_diagonal({0, 7});

    EXPECT_TRUE(neighbors.test({1, 6}));
    EXPECT_EQ(neighbors.count(), 1);
}

TEST(BoardDiagonalNeighbors, Middle)
{
    const auto neighbors = BitBoard::neighbors_diagonal({4, 4});

    EXPECT_TRUE(neighbors.test({3, 3}));
    EXPECT_TRUE(neighbors.test({3, 5}));
    EXPECT_TRUE(neighbors.test({5, 3}));
    EXPECT_TRUE(neighbors.test({5, 5}));
    EXPECT_EQ(neighbors.count(), 4);
}

TEST(BoardCardinalAndDiagonalNeighbors, Corner)
{
    const auto neighbors = BitBoard::neighbors_cardinal_and_diagonal({0, 7});

    EXPECT_TRUE(neighbors.test({0, 6}));
    EXPECT_TRUE(neighbors.test({1, 7}));
    EXPECT_TRUE(neighbors.test({1, 6}));
    EXPECT_EQ(neighbors.count(), 3);
}

TEST(BoardCardinalAndDiagonalNeighbors, Middle)
{
    const auto neighbors = BitBoard::neighbors_cardinal_and_diagonal({4, 4});

    EXPECT_TRUE(neighbors.test({3, 4}));
    EXPECT_TRUE(neighbors.test({4, 3}));
    EXPECT_TRUE(neighbors.test({5, 4}));
    EXPECT_TRUE(neighbors.test({4, 5}));
    EXPECT_TRUE(neighbors.test({3, 3}));
    EXPECT_TRUE(neighbors.test({3, 5}));
    EXPECT_TRUE(neighbors.test({5, 3}));
    EXPECT_TRUE(neighbors.test({5, 5}));
    EXPECT_EQ(neighbors.count(), 8);
}
