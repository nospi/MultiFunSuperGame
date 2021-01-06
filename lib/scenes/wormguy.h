#pragma once

#include "olcMFSG.h"
#include "scene.h"
#include "pausemenu.h"

class WormGuy : public scene
{
public:
    WormGuy(int id, olcMFSG* game);
    bool Create() override;
    bool Update(float) override;
    bool Destroy() override;
    static int CellSize;
    inline olc::vi2d vCellSize() { return olc::vi2d(CellSize, CellSize); }

private:
    std::vector<olc::vi2d> mWormGuy;
    olc::vi2d mFood;
    int mScore;
    olc::vi2d mDirection;
    bool mGameOver;

    float mTimeStep;
    float mTimeAccumulator;
    bool mHasMoved;

    const olc::vi2d mLeft { -1, 0 };
    const olc::vi2d mRight { 1, 0 };
    const olc::vi2d mUp { 0, -1 };
    const olc::vi2d mDown { 0, 1 };

    void randomiseFoodPos();
    void score();

    PauseMenu mPauseMenu;    
};
