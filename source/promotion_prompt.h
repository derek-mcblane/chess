#pragma once

#include "button.h"
#include "layout.h"
#include "pieces.h"
#include "sprite_map_grid.h"
#include "widget.h"

namespace chess::gui {

class PawnPromotionPrompt : public Widget
{
  public:
    PawnPromotionPrompt(
        Region region, PieceColor color, SpriteGrid<Piece>* pieces_sprite_map, sdl::Texture* pieces_texture
    )
        : region_(region), color_(color), pieces_sprite_map_(pieces_sprite_map), pieces_texture_(pieces_texture)
    {
        constexpr size_t padding = 10;
        constexpr int n_pieces = 4;
        const auto inner_region = remove_margin(region_, padding);
        const auto stride = inner_region.w / n_pieces;

        auto position = Point{inner_region.x, inner_region.y};
        for (auto* button : piece_buttons()) {
            button->region() = {position.x, position.y, stride, inner_region.h};
            position.x += stride;
        }

        queen_button_.set_draw_strategy([this](sdl::Renderer& renderer) {
            draw_piece_in_region(renderer, {color_, PieceType::queen}, queen_button_.region());
        });
        queen_button_.set_on_clicked_callback([this](){

        });

        rook_button_.set_draw_strategy([this](sdl::Renderer& renderer) {
            draw_piece_in_region(renderer, {color_, PieceType::rook}, rook_button_.region());
        });

        bishop_button_.set_draw_strategy([this](sdl::Renderer& renderer) {
            draw_piece_in_region(renderer, {color_, PieceType::bishop}, bishop_button_.region());
        });

        knight_button_.set_draw_strategy([this](sdl::Renderer& renderer) {
            draw_piece_in_region(renderer, {color_, PieceType::knight}, knight_button_.region());
        });
    }

  private:
    void on_button_down_impl(const SDL_MouseButtonEvent& event) override;
    void on_button_up_impl(const SDL_MouseButtonEvent& event) override;

    Region& region_impl() override;
    [[nodiscard]] constexpr const Region& region_impl() const override;

    void draw_impl(sdl::Renderer& renderer) const override;

    std::vector<Button*> piece_buttons()
    {
        return {&queen_button_, &rook_button_, &bishop_button_, &knight_button_};
    }

    void draw_piece_in_region(sdl::Renderer& renderer, const Piece piece, Region region)
    {
        renderer.set_draw_blend_mode(SDL_BLENDMODE_NONE);
        const auto piece_rect = pieces_sprite_map_->get_region(piece);
        renderer.copy<int>(*pieces_texture_, piece_rect, region);
    }

    Region region_;
    PieceColor color_;
    SpriteGrid<Piece>* pieces_sprite_map_;
    sdl::Texture* pieces_texture_;
    Button queen_button_;
    Button rook_button_;
    Button bishop_button_;
    Button knight_button_;
};

} // namespace chess::gui
