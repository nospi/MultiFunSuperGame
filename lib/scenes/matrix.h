#pragma once

#include "scene.h"
#include <vector>
#include <string>
#include <chrono>

struct sColumn;

class Matrix : public scene
{
public:
    static float MaxSpeed;

    Matrix(int id, olcMFSG* game);
    bool Create() override;
    bool Update(float) override;
    bool Destroy() override;

private:
    olcMFSG* mGame;
    char* mGrid = nullptr;
    int mCols;
    int mRows;
    std::vector<olc::Pixel> mSpeedColours;
    std::vector<sColumn> mColumns;
    std::vector<char> mAlphabet;
};