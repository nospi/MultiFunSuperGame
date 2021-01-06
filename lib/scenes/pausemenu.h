#pragma once

#include <functional>
#include "scene.h"
#include "button.h"
#include "rect.h"

class PauseMenu
{
public:
    PauseMenu();
    inline void Show() { mEnabled = true; }
    inline void Hide() { mEnabled = false; }
    inline bool Toggle() { mEnabled = !mEnabled; return mEnabled; }
    inline bool Enabled() const { return mEnabled; }
    bool Create(scene*, olcMFSG*);
    bool Process(float, olcMFSG*);
    
    bool bShouldExit = false;
    scene* mScene;
    std::function<void(void)> onRestart;
    std::function<void(void)> onQuit;
    std::vector<Button> mButtons;

private:
    bool mEnabled = false;
    rect<olc::vi2d> mBounds;
    olc::Pixel mBGColour = olc::DARK_GREY;
    olc::Pixel mFGColour = olc::GREY;
    Button btnResume;
    Button btnRestart;
    Button btnQuit;
};