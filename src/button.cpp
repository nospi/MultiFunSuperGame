#include "button.h"
#include "scene.h"

void Button::DrawSelf(olcMFSG* game)
{
    olc::Pixel colour = bgColour;
    if (bounds.contains(game->GetMousePos()))
        colour *= 1.5f;    

    olc::vi2d vPos;
    vPos.x = bounds.position.x + (bounds.size.x >> 1) - (int)sLabel.length() * 4 * txtScale;
    vPos.y = bounds.position.y + (bounds.size.y >> 1) - 4 * txtScale;
    game->FillRect(bounds.position + olc::vi2d(3, 3), bounds.size, colour * 0.2f);
    game->FillRect(bounds.position, bounds.size, colour);
    game->DrawShadowedString(vPos, { 2, 2 }, sLabel, fgColour, shadowColour, txtScale);
}

Button CreateMenuButton(const std::string& label, rect<olc::vi2d> bounds, std::function<void(void)> onClick)
{
    Button b;
    b.sLabel = label;
    b.onClick = onClick;
    b.bounds = bounds;

    b.bgColour = olc::VERY_DARK_GREY;
    b.fgColour = olc::WHITE;
    b.shadowColour = olc::BLACK;
    b.txtScale = 2U;

    return b;
}