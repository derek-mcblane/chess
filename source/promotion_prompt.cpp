#include "promotion_prompt.h"

#include "sdlpp.h"

namespace chess::gui {

void PawnPromotionPrompt::on_button_down_impl(const SDL_MouseButtonEvent& event)
{
    for (Button* button : piece_buttons()) {
        button->on_button_down(event);
    }
}

void PawnPromotionPrompt::on_button_up_impl(const SDL_MouseButtonEvent& event)
{
    for (Button* button : piece_buttons()) {
        button->on_button_up(event);
    }
}

PawnPromotionPrompt::Region& PawnPromotionPrompt::region_impl()
{
    return region_;
}
[[nodiscard]] constexpr const PawnPromotionPrompt::Region& PawnPromotionPrompt::region_impl() const
{
    return region_;
}

void PawnPromotionPrompt::draw_impl(sdl::Renderer& renderer) const
{
    
}

} // namespace chess::gui
