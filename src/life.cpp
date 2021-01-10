#include "scenes/life.h"

int Life::CellSize = 32;

Life::Life(int id, olcMFSG* game) : scene(id, "Conway's Game of Life!", game) {}

bool Life::Create()
{
    mRows = game->ScreenHeight() / CellSize;
    mCols = game->ScreenWidth() / CellSize;
    mGrid = new bool[mRows * mCols]{ false };
    mGeneration = 0;
    mTimeStep = 0.1f;
    mTimeAccumulator = 0.0f;
    mRunning = false;
    mPauseMenu.Create(this, this->game);
    return true;
}

bool Life::Update(float fElapsedTime)
{
    // functional user input
    if (game->GetKey(olc::ESCAPE).bPressed)
        mPauseMenu.Toggle();

    if (!mPauseMenu.Enabled())
    {
        if (!mRunning && game->GetKey(olc::C).bPressed)
            for (int i = 0; i < mRows * mCols; i++)
            {
                mGrid[i] = false;
                mGeneration = 0;
            } 
        if (game->GetKey(olc::NP_ADD).bPressed)
            mTimeStep -= mTimeStep * 0.1f;
        if (game->GetKey(olc::NP_SUB).bPressed)
            mTimeStep += mTimeStep * 0.1f;           
        
        // handle simulation pause toggle
        if (game->GetKey(olc::SPACE).bPressed)
            mRunning = !mRunning;

        // handle user cell toggle
        if (!mRunning)
            if (game->GetMouse(0).bPressed)
            {
                int x = game->GetMouseX() / CellSize;
                int y = game->GetMouseY() / CellSize;
                mGrid[y * mCols + x] = !mGrid[y * mCols + x];
            }

        // accumulate time and tick if necessary
        if (mRunning)
        {
            mTimeAccumulator += fElapsedTime;
            if (mTimeAccumulator >= mTimeStep)
            {
                mTimeAccumulator -= mTimeStep;
                Tick();
            }
        }
    }

    game->Clear(0);

    // draw grid
    for (int y = 0; y < mRows; y++)
        for (int x = 0; x < mCols; x++)
        {
            if (mGrid[y * mCols + x])
            {
                olc::vi2d pos;
                pos.x = x * CellSize + (CellSize >> 1);
                pos.y = y * CellSize + (CellSize >> 1);
                game->FillCircle(pos, (CellSize >> 1) - 2, olc::GREEN);
            }
        }

    // draw mouse cursor
    if (!mRunning)
    {
        olc::vi2d cursor = game->GetMousePos();
        cursor.x = floorf((float)cursor.x / (float)CellSize) * CellSize + (CellSize >> 1);
        cursor.y = floorf((float)cursor.y / (float)CellSize) * CellSize + (CellSize >> 1);
        game->DrawCircle(cursor, CellSize >> 1, olc::RED);
    }

    // ui
    game->DrawShadowedString({ 10, 10 }, { 1, 1 }, "Generation: " + std::to_string(mGeneration), olc::WHITE, olc::BLACK, 2U);
    game->DrawShadowedString({ 10, 42 }, { 1, 1 }, "Time Step: " + std::to_string(mTimeStep * 1000.0f) + "ms", olc::WHITE, olc::BLACK, 1U);
    game->DrawShadowedString({ 10, 58 }, { 1, 1 }, "Ticks/Sec: " + std::to_string(1.0f / mTimeStep), olc::WHITE, olc::BLACK, 1U);

    return mPauseMenu.Process(fElapsedTime);
}

bool Life::Destroy()
{
    if (mGrid != nullptr)
        delete[] mGrid;
    return true;
}

void Life::Tick()
{
    bool* temp = new bool[mRows * mCols]{ false };
    for (int i = 0; i < mRows * mCols; i++)
        temp[i] = mGrid[i];

    for (int y = 1; y < mRows - 1; y++)
        for (int x = 1; x < mCols - 1; x++)
        {
            int liveNeighbours = 0;
            for (int j = -1; j < 2; j++)
                for (int k = -1; k < 2; k++)
                    if (!(j == 0 && k == 0))
                        if (mGrid[(y + j) * mCols + (x + k)])
                            liveNeighbours++;
            
            bool alive = mGrid[y * mCols + x];

            if (alive && liveNeighbours > 3)
                temp[y * mCols + x] = false;
            else if (alive && liveNeighbours < 2)
                temp[y * mCols + x] = false;
            else if (!alive && liveNeighbours == 3)
                temp[y * mCols + x] = true;
            else
                temp[y * mCols + x] = mGrid[y * mCols + x];
        }
    
    for (int i = 0; i < mCols * mRows; i++)
        mGrid[i] = temp[i];

    mGeneration++;
    delete[] temp;
}