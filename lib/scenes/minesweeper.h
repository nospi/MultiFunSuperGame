#pragma once

#include "olcMFSG.h"
#include "scene.h"
#include "pausemenu.h"
#include <vector>
#include <string>

class MineSweeper : public scene
{
public:
    struct cell
    {
        olc::vi2d position;
        bool enabled = true;
        bool explosive = false;
        bool flagged = false;
        int value = 0;
        cell();
        cell(int, int);
    };

public:
    MineSweeper(int, olcMFSG*);
    bool Create() override;
    bool Update(float) override;

private:
    int nCols = 0;
    int nRows = 0;
    int nMines = 30;
    int nSolved = 0;
    olc::vi2d vCellSize;
    std::vector<cell> vCells;
    bool bGameOver = false;
    bool bFirstClick = true;
    PauseMenu mPauseMenu;

private:
    void drawCell(const cell&);
    void clickCell(cell&);
    void calculateValue(cell&);
};