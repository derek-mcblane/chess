#include "gtest/gtest.h"

#include "bit_board.h"

#include <algorithm>
#include <bitset>
#include <iterator>

class BitBoardShiftTest : public ::testing::Test
{};

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

TEST(BoardSetBit, Single)
{
    BitBoard board;
    board.set({4, 2});
    EXPECT_EQ(
        board.to_string(),
        "00000000"
        "00000000"
        "00000000"
        "00000000"
        "00100000"
        "00000000"
        "00000000"
        "00000000"
    );
}

TEST(BoardSetBit, Checkered)
{
    BitBoard bits;
    for (int row = 0; row < BitBoard::board_size; row += 1) {
        for (int col = (row % 2 == 0) ? 0 : 1; col < BitBoard::board_size; col += 2) {
            bits.set({row, col});
        }
    }
    EXPECT_EQ(bits.to_string(), test_board.to_string());
}

TEST(BoardTestBit, Checkered)
{
    BitBoard bits{test_board};
    for (int row = 0; row < BitBoard::board_size; row += 1) {
        for (int col = (row % 2 == 0) ? 0 : 1; col < BitBoard::board_size; col += 2) {
            EXPECT_TRUE(bits.test({row, col}));
        }
        for (int col = (row % 2 == 0) ? 1 : 0; col < BitBoard::board_size; col += 2) {
            EXPECT_FALSE(bits.test({row, col}));
        }
    }
}

TEST(BoardTestAny, True)
{
    static const BitBoard square{std::bitset<64>{"00000000"
                                                 "00000000"
                                                 "00111100"
                                                 "00111100"
                                                 "00111100"
                                                 "00111100"
                                                 "00000000"
                                                 "00000000"}};
    EXPECT_TRUE(test_board.test_any(square));
    EXPECT_TRUE(square.test_any(test_board));
}

TEST(BoardTestAny, False)
{
    static const BitBoard left_board{std::bitset<64>{"11110000"
                                                     "11110000"
                                                     "11110000"
                                                     "11110000"
                                                     "11110000"
                                                     "11110000"
                                                     "11110000"
                                                     "11110000"}};

    static const BitBoard right_board{std::bitset<64>{"00001111"
                                                      "00001111"
                                                      "00001111"
                                                      "00001111"
                                                      "00001111"
                                                      "00001111"
                                                      "00001111"
                                                      "00001111"}};
    EXPECT_FALSE(left_board.test_any(right_board));
    EXPECT_FALSE(right_board.test_any(left_board));
}

TEST(Board, TestAll)
{
    EXPECT_TRUE(test_board.test_all(test_board));
}

TEST(Board, SetBoard)
{
    BitBoard bits;
    bits.set(test_board);
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

TEST(BoardStaticShift, ShiftRightN)
{
    EXPECT_EQ(
        test_board.shift<Direction::right>(5).to_string(),
        "00000101"
        "00000010"
        "00000101"
        "00000010"
        "00000101"
        "00000010"
        "00000101"
        "00000010"
    );
}

TEST(BoardStaticShift, ShiftUpRightN)
{
    EXPECT_EQ(
        test_board.shift<Direction::upright>(5).to_string(),
        "00000010"
        "00000101"
        "00000010"
        "00000000"
        "00000000"
        "00000000"
        "00000000"
        "00000000"
    );
}

TEST(BoardStaticShift, ShiftUpN)
{
    EXPECT_EQ(
        test_board.shift<Direction::up>(5).to_string(),
        "01010101"
        "10101010"
        "01010101"
        "00000000"
        "00000000"
        "00000000"
        "00000000"
        "00000000"
    );
}

TEST(BoardStaticShift, ShiftUpLeftN)
{
    EXPECT_EQ(
        test_board.shift<Direction::upleft>(5).to_string(),
        "10100000"
        "01000000"
        "10100000"
        "00000000"
        "00000000"
        "00000000"
        "00000000"
        "00000000"
    );
}

TEST(BoardStaticShift, ShiftLeftN)
{
    EXPECT_EQ(
        test_board.shift<Direction::left>(5).to_string(),
        "01000000"
        "10100000"
        "01000000"
        "10100000"
        "01000000"
        "10100000"
        "01000000"
        "10100000"
    );
}

TEST(BoardStaticShift, ShiftDownLeftN)
{
    EXPECT_EQ(
        test_board.shift<Direction::downleft>(5).to_string(),
        "00000000"
        "00000000"
        "00000000"
        "00000000"
        "00000000"
        "01000000"
        "10100000"
        "01000000"
    );
}

TEST(BoardStaticShift, ShiftDownN)
{
    EXPECT_EQ(
        test_board.shift<Direction::down>(5).to_string(),
        "00000000"
        "00000000"
        "00000000"
        "00000000"
        "00000000"
        "10101010"
        "01010101"
        "10101010"
    );
}

TEST(BoardStaticShift, ShiftDownRightN)
{
    EXPECT_EQ(
        test_board.shift<Direction::downright>(5).to_string(),
        "00000000"
        "00000000"
        "00000000"
        "00000000"
        "00000000"
        "00000101"
        "00000010"
        "00000101"
    );
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
