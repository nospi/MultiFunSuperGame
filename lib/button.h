#pragma once
#ifndef BUTTON_H
#define BUTTON_H

#include <functional>
#include "olcMFSG.h"
#include "rect.h"

struct Button
{
    std::string sLabel;
    uint32_t txtScale = 1U;
    rect<olc::vi2d> bounds;
    olc::Pixel bgColour;
    olc::Pixel fgColour;
    olc::Pixel shadowColour;
    // void(*onClick)(void) = nullptr;
    std::function<void(void)> onClick;
    void DrawSelf(olcMFSG* game);
};

Button CreateMenuButton(const std::string&, rect<olc::vi2d>, std::function<void(void)>);

Button CreateLoadSceneButton(scene* scn, rect<olc::vi2d> bounds, olcMFSG* game);

#endif /* BUTTON_H */