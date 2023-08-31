#pragma once

#include "bit_board.h"
#include "pieces.h"
#include "vec2.h"

#include <optional>
#include <set>
#include <utility>

namespace chess {

static constexpr BitBoard::Position::dimension_type black_piece_row_index{0};
static constexpr BitBoard::Position::dimension_type black_pawn_row_index{1};
static constexpr BitBoard::Position::dimension_type white_piece_row_index{7};
static constexpr BitBoard::Position::dimension_type white_pawn_row_index{6};

// static constexpr auto black_piece_row = BitBoard::make_row(black_piece_row_index);

template <PieceColor Color>
BitBoard piece_row()
{
    if constexpr (Color == PieceColor::black) {
        return BitBoard::make_row(black_piece_row_index);
    } else {
        return BitBoard::make_row(white_piece_row_index);
    }
}

template <PieceColor Color>
BitBoard pawn_row()
{
    if constexpr (Color == PieceColor::black) {
        return BitBoard::make_row(black_pawn_row_index);
    } else {
        return BitBoard::make_row(white_pawn_row_index);
    }
}

class BoardPieces
{
  public:
    using Position = BitBoard::Position;

    struct Move
    {
        Position from;
        Position to;
    };

    struct BitBoardMove
    {
        BitBoard from;
        BitBoard to;
        constexpr static BitBoardMove from_move(const Move move)
        {
            return {BitBoard{move.from}, BitBoard{move.to}};
        }
    };

    [[nodiscard]] static BoardPieces make_standard_setup_board();

    [[nodiscard]] constexpr BitBoard pawns() const noexcept
    {
        return pawns_;
    }
    [[nodiscard]] constexpr BitBoard knights() const noexcept
    {
        return knights_;
    }
    [[nodiscard]] constexpr BitBoard bishops() const noexcept
    {
        return bishops_;
    }
    [[nodiscard]] constexpr BitBoard rooks() const noexcept
    {
        return rooks_;
    }
    [[nodiscard]] constexpr BitBoard queens() const noexcept
    {
        return queens_;
    }
    [[nodiscard]] constexpr BitBoard kings() const noexcept
    {
        return kings_;
    }

    [[nodiscard]] constexpr BitBoard black() const noexcept
    {
        return black_;
    }
    [[nodiscard]] constexpr BitBoard white() const noexcept
    {
        return white_;
    }

    [[nodiscard]] BitBoard occupied() const noexcept
    {
        return black_ | white_;
    }

    template <PieceColor Color>
    [[nodiscard]] constexpr const BitBoard& of() const noexcept
    {
        if constexpr (Color == PieceColor::black) {
            return black_;
        } else {
            return white_;
        }
    }
    [[nodiscard]] constexpr const BitBoard& of(const PieceColor color) const noexcept
    {
        switch (color) {
        case PieceColor::black:
            return black_;
        case PieceColor::white:
            return white_;
        }
    }

    template <PieceType Type>
    [[nodiscard]] constexpr const BitBoard& of() const noexcept
    {
        if constexpr (Type == PieceType::pawn) {
            return pawns_;
        } else if constexpr (Type == PieceType::knight) {
            return knights_;
        } else if constexpr (Type == PieceType::bishop) {
            return bishops_;
        } else if constexpr (Type == PieceType::rook) {
            return rooks_;
        } else if constexpr (Type == PieceType::queen) {
            return queens_;
        } else if constexpr (Type == PieceType::king) {
            return kings_;
        }
    }
    [[nodiscard]] constexpr const BitBoard& of(const PieceType type) const noexcept
    {
        switch (type) {
        case PieceType::pawn:
            return pawns_;
        case PieceType::knight:
            return knights_;
        case PieceType::bishop:
            return bishops_;
        case PieceType::rook:
            return rooks_;
        case PieceType::queen:
            return queens_;
        case PieceType::king:
            return kings_;
        }
    }

    template <Piece Piece>
    [[nodiscard]] constexpr BitBoard of() const noexcept
    {
        return of<Piece.color>() & of<Piece.type>();
    }
    [[nodiscard]] constexpr BitBoard of(const Piece piece) const noexcept
    {
        return of(piece.color) & of(piece.type);
    }

    [[nodiscard]] std::optional<Piece> at(BitBoard position) const noexcept;
    [[nodiscard]] std::optional<Piece> at(const Position& position) const;
    [[nodiscard]] Piece at_checked(BitBoard position) const;
    [[nodiscard]] Piece at_checked(const Position& position) const;
    [[nodiscard]] std::optional<PieceColor> color_at(BitBoard position) const noexcept;
    [[nodiscard]] std::optional<PieceColor> color_at(const Position& position) const;
    [[nodiscard]] std::optional<PieceType> type_at(BitBoard position) const noexcept;
    [[nodiscard]] std::optional<PieceType> type_at(const Position& position) const;

    template <Piece Piece>
    void clear(const BitBoard board) noexcept
    {
        of<Piece.color>().clear(board);
        of<Piece.type>().clear(board);
    }
    void clear(const Piece piece, const BitBoard board) noexcept
    {
        of(piece.color).clear(board);
        of(piece.type).clear(board);
    }
    void clear(const Position& position)
    {
        clear(BitBoard{position});
    }

    void clear(BitBoard board) noexcept
    {
        black_.clear(board);
        white_.clear(board);
        pawns_.clear(board);
        knights_.clear(board);
        bishops_.clear(board);
        rooks_.clear(board);
        queens_.clear(board);
        kings_.clear(board);
    }

    template <Piece Piece>
    void set(const BitBoard positions)
    {
        clear(positions);
        of<Piece.type>().set(positions);
        of<Piece.color>().set(positions);
    }
    template <Piece Piece>
    void set(const Position& position)
    {
        set<Piece>(BitBoard{position});
    }

    void set(const Piece piece, const BitBoard positions) noexcept
    {
        clear(positions);
        of(piece.type).set(positions);
        of(piece.color).set(positions);
    }
    void set(const Piece piece, const Position& position)
    {
        set(piece, BitBoard{position});
    }

    template <Piece Piece>
    void move(const BitBoardMove move)
    {
        assert(at_checked(move.from) == Piece);
        clear<Piece>(move.from);
        set<Piece>(move.to);
        assert(!at(move.from).has_value());
        assert(at_checked(move.to) == Piece);
    }
    void move(const Piece piece, const BitBoardMove move)
    {
        assert(at_checked(move.from) == piece);
        clear(piece, move.from);
        set(piece, move.to);
        assert(!at(move.from).has_value());
        assert(at_checked(move.to) == piece);
    }

    template <Direction D>
    [[nodiscard]] BitBoard sliding_moves(BitBoard from, size_t range = BitBoard::board_size) const
    {
        auto moves = BitBoard{from};
        for (size_t distance = 0; distance < range; ++distance) {
            moves.dilate<D>();
            if ((moves & ~from).test_any(occupied()) || moves.on_edge<D>()) {
                break;
            }
        }
        return moves.clear(from);
    }
    [[nodiscard]] BitBoard sliding_moves(Direction direction, BitBoard from, size_t range = BitBoard::board_size) const;
    template <typename DirectionRange>
    [[nodiscard]] BitBoard
    sliding_moves(DirectionRange&& directions, BitBoard from, size_t range = BitBoard::board_size) const
    {
        BitBoard moves;
        for (const auto direction : std::forward<DirectionRange>(directions)) {
            moves.set(sliding_moves(direction, from, range));
        }
        return moves;
    }

  private:
    BitBoard pawns_;
    BitBoard knights_;
    BitBoard bishops_;
    BitBoard rooks_;
    BitBoard queens_;
    BitBoard kings_;
    BitBoard black_;
    BitBoard white_;

    template <PieceColor Color>
    constexpr BitBoard& of()
    {
        return const_cast<BitBoard&>(std::as_const(*this).of<Color>());
    }
    constexpr BitBoard& of(const PieceColor color)
    {
        return const_cast<BitBoard&>(std::as_const(*this).of(color));
    }

    template <PieceType Type>
    constexpr BitBoard& of()
    {
        return const_cast<BitBoard&>(std::as_const(*this).of<Type>());
    }
    constexpr BitBoard& of(const PieceType type)
    {
        return const_cast<BitBoard&>(std::as_const(*this).of(type));
    }
};

} // namespace chess
