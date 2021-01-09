#include "scenes/wormguy.h"
#include <iostream>

int WormGuy::CellSize = 16;

WormGuy::WormGuy(int id, olcMFSG* game) : scene(id, "WormGuy", game) {}

bool WormGuy::Create()
{
    // initialise starting values
    mTimeStep = 0.085f;
    mTimeAccumulator = 0.0f;
    mHasMoved = false;
    mScore = 0;
    mGameOver = false;
    randomiseFoodPos();
    mWormGuy.clear();

    // randomise head position
    olc::vi2d pos;
    pos.x = rand() % (game->ScreenWidth() / CellSize - 4) + 2;
    pos.y = rand() % (game->ScreenHeight() / CellSize - 4) + 2;
    mWormGuy.push_back(pos);

    // create starting body
    for (int i = 1; i < 8; i++)
        mWormGuy.emplace_back(pos.x - i, pos.y);

    // determine starting direction
    mDirection = mRight;

    // configure pause menu
    mPauseMenu.Create(this, this->game);
    
    return true;
}

bool WormGuy::Update(float fElapsedTime)
{
    if (game->GetKey(olc::ESCAPE).bPressed)
        mPauseMenu.Toggle();
 
    if (mGameOver)
        mPauseMenu.Show();

    if (!mPauseMenu.Enabled() && ! mGameOver)
    {
        // handle input
        if (!mHasMoved)
        {
            if (game->GetKey(olc::W).bPressed || game->GetKey(olc::UP).bPressed)
                if (mDirection.dot(mUp) == 0)
                {
                    mDirection = mUp;
                    mHasMoved = true;
                }
            if (game->GetKey(olc::A).bPressed || game->GetKey(olc::LEFT).bPressed)
                if (mDirection.dot(mLeft) == 0)
                {
                    mDirection = mLeft;
                    mHasMoved = true;
                }
            if (game->GetKey(olc::S).bPressed || game->GetKey(olc::DOWN).bPressed)
                if (mDirection.dot(mDown) == 0)
                {
                    mDirection = mDown;
                    mHasMoved = true;
                }
            if (game->GetKey(olc::D).bPressed || game->GetKey(olc::RIGHT).bPressed)
                if (mDirection.dot(mRight) == 0)
                {
                    mDirection = mRight;
                    mHasMoved = true;
                }
        }

        mTimeAccumulator += fElapsedTime;
        if (mTimeAccumulator >= mTimeStep)
        {
            mTimeAccumulator -= mTimeStep;

            // future pos
            olc::vf2d vNextHead = mWormGuy.front() + mDirection;

            // body collisions
            for (auto& cell : mWormGuy)
                if (&cell != &mWormGuy.front())
                    if (cell == vNextHead)
                        mGameOver = true;
            
            // wall collisions
            if (vNextHead.x < 0 || vNextHead.x >= game->ScreenWidth() / CellSize || vNextHead.y < 0 || vNextHead.y >= game->ScreenHeight() / CellSize)
                mGameOver = true;

            if (!mGameOver)
            {
                // move wormguy
                for (int i = mWormGuy.size() - 1; i > 0; i--)
                    mWormGuy[i] = mWormGuy[i - 1];
                mWormGuy[0] += mDirection;

                // check for food collisions
                if (mWormGuy.front() == mFood)
                    score();
            }
            
            mHasMoved = false;
        }
    }

    // draw yo
    game->Clear(0);
    game->FillRect(mFood * vCellSize(), vCellSize(), olc::YELLOW);
    for (auto& cell : mWormGuy)
        game->FillRect(cell * vCellSize(), vCellSize(), &cell == &mWormGuy.front() ? olc::WHITE : olc::GREY);
    
    // ui
    game->DrawShadowedString({ 10, 10 }, { 2, 2 }, "Score: " + std::to_string(mScore), olc::WHITE, olc::BLACK, 3U);

    if (mGameOver)
    {
        std::string sGameOver = "Game Over!";
        olc::vi2d pos = { game->ScreenWidth() / 2 - (int)sGameOver.length() * 16, game->ScreenHeight() / 4 };
        game->DrawShadowedString(pos, { 3, 3 }, sGameOver, olc::YELLOW, olc::RED, 4U);
    }

    // pause menu
    if (!mPauseMenu.Process(fElapsedTime))
        return false;

    return true;
}

bool WormGuy::Destroy()
{
    return true;
}

void WormGuy::randomiseFoodPos()
{
    mFood = olc::vi2d(rand() % (game->ScreenWidth() / CellSize - 2) + 1, rand() % (game->ScreenHeight() / CellSize - 2) + 1);
}

void WormGuy::score()
{
    mScore++;
    randomiseFoodPos();

    // append new tail pos
    olc::vi2d pos = mWormGuy.back();
    olc::vi2d dir = mWormGuy[mWormGuy.size() - 2] - mWormGuy.back();
    pos += dir;
    mWormGuy.push_back(pos);
    pos += dir;
    mWormGuy.push_back(pos);

    mTimeStep -= mTimeStep * 0.033f;
}