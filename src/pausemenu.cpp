#include "scenes/pausemenu.h"

PauseMenu::PauseMenu() {}

bool PauseMenu::Create(scene* scene, olcMFSG* game)
{
    mGame = game;
    mScene = scene;
    mBounds.size = { 320, 240 };
    mBounds.position = { (mGame->ScreenWidth() >> 1) - 160, (mGame->ScreenHeight() >> 1) - 120 };

    olc::vi2d vBtnSize = { 300, 40 };
    olc::vi2d vBtnOffset = { 10, 90 };
    
    btnResume = CreateMenuButton("Resume", rect<olc::vi2d>(mBounds.position + vBtnOffset, vBtnSize), [&](){ Hide(); });
    vBtnOffset.y += 50;
    btnRestart = CreateMenuButton("Restart", rect<olc::vi2d>(mBounds.position + vBtnOffset, vBtnSize), [&](){ mScene->Create(); Hide(); });
    vBtnOffset.y += 50;
    btnQuit = CreateMenuButton("Quit", rect<olc::vi2d>(mBounds.position + vBtnOffset, vBtnSize), [&](){ bShouldExit = true; Hide(); });

    mButtons.push_back(btnResume);
    mButtons.push_back(btnRestart);
    mButtons.push_back(btnQuit);

    return true;
}

bool PauseMenu::Process(float fElapsedTime)
{
    // handle scene exit - need to find a nicer way to do this
    if (bShouldExit)
    {
        bShouldExit = false;
        return false;
    }

    // bail out early if the menu is not enabled
    if (!mEnabled) return true;

    // handle button clicks
    if (mGame->GetMouse(0).bPressed)
        for (auto& btn : mButtons)
            if (btn.bounds.contains(mGame->GetMousePos()))
            {
                btn.onClick();
                break;
            }

    // determine title string position
    uint32_t scale = 4U;
    std::string sTitle = "Paused";
    olc::vi2d vPos;
    vPos.x = mBounds.position.x + (mBounds.size.x >> 1) - sTitle.length() * 4 * scale;
    vPos.y = mBounds.position.y + 8 * scale;

    // draw
    mGame->FillRect(mBounds.position, mBounds.size, mBGColour);
    mGame->DrawShadowedString(vPos, { 2, 2 }, sTitle, olc::WHITE, olc::BLACK, scale);
    for (auto& btn : mButtons)
        btn.DrawSelf(mGame);

    return true;
}