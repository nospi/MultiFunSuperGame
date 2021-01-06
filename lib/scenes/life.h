#pragma once

#include "scene.h"
#include "pausemenu.h"

class Life : public scene
{
public:
    Life(int id, olcMFSG* game);
    bool Create() override;
    bool Update(float) override;
    bool Destroy() override;
    void Tick();
    static int CellSize;

private:
    int mRows = 0;
    int mCols = 0;
    bool* mGrid = nullptr;
    int mGeneration = 0;
    bool mRunning = false;
    float mTimeStep = 0.1f;
    float mTimeAccumulator = 0.0f;
    PauseMenu mPauseMenu;
};
