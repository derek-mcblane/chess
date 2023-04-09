#pragma once

#include "bit_board.h"
#include "vec2.h"

#include <cassert>
#include <map>
#include <optional>

namespace chess {

enum class PieceColor : int
{
    black,
    white,
};
extern const std::map<PieceColor, std::string> piece_color_names;

inline PieceColor opposite_color(const PieceColor color)
{
    switch (color) {
    case PieceColor::black:
        return PieceColor::white;
    case PieceColor::white:
        return PieceColor::black;
    }
}

enum class PieceType : int
{
    pawn,
    knight,
    bishop,
    rook,
    queen,
    king,
};
extern const std::map<PieceType, std::string> piece_type_names;

struct Piece
{
    PieceColor color;
    PieceType type;
};

namespace pieces {
constexpr auto black_pawn = Piece{PieceColor::black, PieceType::pawn};
constexpr auto black_knight = Piece{PieceColor::black, PieceType::knight};
constexpr auto black_bishop = Piece{PieceColor::black, PieceType::bishop};
constexpr auto black_rook = Piece{PieceColor::black, PieceType::rook};
constexpr auto black_queen = Piece{PieceColor::black, PieceType::queen};
constexpr auto black_king = Piece{PieceColor::black, PieceType::king};
constexpr auto white_pawn = Piece{PieceColor::white, PieceType::pawn};
constexpr auto white_knight = Piece{PieceColor::white, PieceType::knight};
constexpr auto white_bishop = Piece{PieceColor::white, PieceType::bishop};
constexpr auto white_rook = Piece{PieceColor::white, PieceType::rook};
constexpr auto white_queen = Piece{PieceColor::white, PieceType::queen};
constexpr auto white_king = Piece{PieceColor::white, PieceType::king};
} // namespace pieces

inline bool operator==(const Piece& lhs, const Piece& rhs)
{
    return (lhs.type == rhs.type) && (lhs.color == rhs.color);
}

inline bool operator!=(const Piece& lhs, const Piece& rhs)
{
    return !(lhs == rhs);
}

inline bool operator<(const Piece& lhs, const Piece& rhs)
{
    const auto lhs_type = static_cast<std::underlying_type_t<PieceType>>(lhs.type);
    const auto lhs_color = static_cast<std::underlying_type_t<PieceColor>>(lhs.color);
    const auto rhs_type = static_cast<std::underlying_type_t<PieceType>>(rhs.type);
    const auto rhs_color = static_cast<std::underlying_type_t<PieceColor>>(rhs.color);

    if (lhs_type == rhs_type) {
        return lhs_color < rhs_color;
    }
    return lhs_type < rhs_type;
}

inline bool operator>=(const Piece& lhs, const Piece& rhs)
{
    return !(rhs < lhs);
}

inline bool operator>(const Piece& lhs, const Piece& rhs)
{
    return rhs < lhs;
}

inline bool operator<=(const Piece& lhs, const Piece& rhs)
{
    return !(rhs > lhs);
}

class BoardPieces
{
  public:
    using Position = BitBoard::Position;

    void set_piece(Piece piece, const Position& position);
    void move(Position from, Position to);
    void clear_piece(const Position& position);
    [[nodiscard]] bool occupied(const Position& position) const;
    [[nodiscard]] std::optional<PieceColor> piece_color_at(const Position& position) const;
    [[nodiscard]] std::optional<PieceType> piece_type_at(const Position& position) const;
    [[nodiscard]] std::optional<Piece> piece_at(const Position& position) const;
    [[nodiscard]] bool is_valid_move(Position from, Position to);
    [[nodiscard]] BitBoard valid_moves(Position from);
    [[nodiscard]] PieceColor active_color() const;
    [[nodiscard]] bool is_active_piece(const Position& position) const;

    [[nodiscard]] static BoardPieces make_standard_setup_board();

  private:
    struct PieceMove
    {
        Piece piece;
        BitBoard from;
        BitBoard to;
    };

    inline static constexpr Position::dimension_type black_piece_row{0};
    inline static constexpr Position::dimension_type black_pawn_row{1};
    inline static constexpr Position::dimension_type white_piece_row{7};
    inline static constexpr Position::dimension_type white_pawn_row{6};
    inline static constexpr Position::dimension_type kingside_rook_col{0};
    inline static constexpr Position::dimension_type queenside_rook_col{7};

    /*
    inline static constexpr BitBoard black_king_position{0x08'00'00'00'00'00'00'00U};
    inline static constexpr BitBoard black_kingside_rook_position{0x80'00'00'00'00'00'00'00U};
    inline static constexpr BitBoard black_queenside_rook_position{0x01'00'00'00'00'00'00'00U};
    inline static constexpr BitBoard black_castle_kingside_king_move{0x02'00'00'00'00'00'00'00U};
    inline static constexpr BitBoard black_castle_queenside_king_move{0x20'00'00'00'00'00'00'00U};
    inline static constexpr BitBoard black_castle_kingside_rook_move{0x04'00'00'00'00'00'00'00U};
    inline static constexpr BitBoard black_castle_queenside_rook_move{0x10'00'00'00'00'00'00'00U};

    inline static constexpr BitBoard white_king_position{0x00'00'00'00'00'00'00'08U};
    inline static constexpr BitBoard white_kingside_rook_position{0x00'00'00'00'00'00'00'80U};
    inline static constexpr BitBoard white_queenside_rook_position{0x00'00'00'00'00'00'00'01U};
    inline static constexpr BitBoard white_castle_kingside_king_move{0x00'00'00'00'00'00'00'02U};
    inline static constexpr BitBoard white_castle_queenside_king_move{0x00'00'00'00'00'00'00'20U};
    inline static constexpr BitBoard white_castle_kingside_rook_move{0x00'00'00'00'00'00'00'04U};
    inline static constexpr BitBoard white_castle_queenside_rook_move{0x00'00'00'00'00'00'00'10U};
    */

    inline static constexpr Position black_king_position{0, 4};
    inline static constexpr Position black_kingside_rook_position{0, 7};
    inline static constexpr Position black_queenside_rook_position{0, 0};
    inline static constexpr Position black_castle_kingside_king_move{0, 6};
    inline static constexpr Position black_castle_queenside_king_move{0, 2};
    inline static constexpr Position black_castle_kingside_rook_move{0, 5};
    inline static constexpr Position black_castle_queenside_rook_move{0, 3};

    inline static constexpr Position white_king_position{7, 4};
    inline static constexpr Position white_kingside_rook_position{7, 7};
    inline static constexpr Position white_queenside_rook_position{7, 0};
    inline static constexpr Position white_castle_kingside_king_move{7, 6};
    inline static constexpr Position white_castle_queenside_king_move{7, 2};
    inline static constexpr Position white_castle_kingside_rook_move{7, 5};
    inline static constexpr Position white_castle_queenside_rook_move{7, 3};

    std::vector<PieceMove> move_history_;
    std::map<Position, BitBoard> attacked_by_;
    BitBoard attacked_;
    BitBoard en_passant_square_;
    BitBoard pawns_;
    BitBoard knights_;
    BitBoard bishops_;
    BitBoard rooks_;
    BitBoard queens_;
    BitBoard kings_;
    BitBoard black_;
    BitBoard white_;
    PieceColor active_color_{PieceColor::white};
    bool black_queenside_castle_piece_moved_{false};
    bool black_kingside_castle_piece_moved_{false};
    bool white_queenside_castle_piece_moved_{false};
    bool white_kingside_castle_piece_moved_{false};

    [[nodiscard]] bool occupied(BitBoard position) const;
    [[nodiscard]] std::optional<Piece> piece_at(BitBoard position) const;
    [[nodiscard]] Piece piece_at_checked(BitBoard position) const;
    [[nodiscard]] BitBoard occupied_board() const;
    [[nodiscard]] BitBoard attacked_board() const;
    [[nodiscard]] std::optional<PieceType> piece_type_at(BitBoard position) const;
    [[nodiscard]] bool is_pawn(BitBoard position) const;
    [[nodiscard]] bool is_knight(BitBoard position) const;
    [[nodiscard]] bool is_bishop(BitBoard position) const;
    [[nodiscard]] bool is_rook(BitBoard position) const;
    [[nodiscard]] bool is_queen(BitBoard position) const;
    [[nodiscard]] bool is_king(BitBoard position) const;
    [[nodiscard]] std::optional<PieceColor> piece_color_at(BitBoard position) const;
    [[nodiscard]] bool is_black(BitBoard position) const;
    [[nodiscard]] bool is_white(BitBoard position) const;
    void clear_pieces(BitBoard board);
    void set_pieces(Piece piece, BitBoard positions);
    void move_piece(PieceMove move);
    void move(BitBoard from, BitBoard to);
    void set_squares_attacked_by(const Position& position);
    void clear_squares_attacked_by(const Position& position);
    void update_en_passant_state(PieceMove move);
    void update_castling_state(PieceMove move);

    void move_pawn(BitBoard from, BitBoard to);
    void move_knight(BitBoard from, BitBoard to);
    void move_bishop(BitBoard from, BitBoard to);
    void move_rook(BitBoard from, BitBoard to);
    void move_queen(BitBoard from, BitBoard to);
    void move_king(BitBoard from, BitBoard to);
    void move_black(BitBoard from, BitBoard to);
    void move_white(BitBoard from, BitBoard to);

    [[nodiscard]] BitBoard pawn_moves(Position from) const;
    [[nodiscard]] BitBoard knight_moves(Position from) const;
    [[nodiscard]] BitBoard bishop_moves(Position from) const;
    [[nodiscard]] BitBoard rook_moves(Position from) const;
    [[nodiscard]] BitBoard queen_moves(Position from) const;
    [[nodiscard]] bool white_can_castle_kingside() const;
    [[nodiscard]] bool white_can_castle_queenside() const;
    [[nodiscard]] bool black_can_castle_kingside() const;
    [[nodiscard]] bool black_can_castle_queenside() const;
    [[nodiscard]] bool can_castle(BitBoard between_squares, BitBoard king_squares) const;
    [[nodiscard]] BitBoard black_king_castling_moves() const;
    [[nodiscard]] BitBoard white_king_castling_moves() const;
    [[nodiscard]] BitBoard king_castling_moves() const;
    [[nodiscard]] BitBoard king_moves(Position from) const;
    [[nodiscard]] BitBoard valid_moves_bitboard(Position from) const;

    template <Direction D>
    [[nodiscard]] BitBoard sliding_moves(BitBoard from, size_t range = BitBoard::board_size) const;
    [[nodiscard]] BitBoard sliding_moves(BitBoard from, Direction direction, size_t range = BitBoard::board_size) const;
    template <typename DirectionRange>
    [[nodiscard]] BitBoard
    sliding_moves(Position from, DirectionRange&& directions, size_t range = BitBoard::board_size) const;

    [[nodiscard]] bool is_pawn_start_square(Position from) const;
    void remove_if_color(BitBoard& moves, PieceColor color) const;
    [[nodiscard]] BitBoard board_of_color(PieceColor color) const;
    [[nodiscard]] BitBoard& board_of_color(PieceColor color);
    [[nodiscard]] BitBoard active_color_board() const;
    [[nodiscard]] BitBoard& active_color_board();
    [[nodiscard]] BitBoard inactive_color_board() const;
    [[nodiscard]] BitBoard& inactive_color_board();
};

template <Direction D>
[[nodiscard]] BitBoard BoardPieces::sliding_moves(const BitBoard from, const size_t range) const
{
    const auto from_color = piece_color_at(from);
    if (!from_color.has_value()) {
        assert(!"sliding moves from empty square");
        return BitBoard{};
    }
    auto moves = from;
    for (size_t distance = 0; distance < range; ++distance) {
        moves.dilate<D>();
        if ((moves & ~from).test_any(occupied_board()) || moves.on_edge<D>()) {
            break;
        }
    }
    return moves.clear(board_of_color(*from_color));
}

template <typename DirectionRange>
[[nodiscard]] BitBoard BoardPieces::sliding_moves(const Position from, DirectionRange&& directions, size_t range) const
{
    BitBoard moves;
    for (const auto direction : std::forward<DirectionRange>(directions)) {
        moves.set(sliding_moves(BitBoard{from}, direction, range));
    }
    return moves.clear(from);
}

} // namespace chess
