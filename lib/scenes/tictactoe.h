#pragma once

#include "olcMFSG.h"
#include "scene.h"
#include "rect.h"
#include "pausemenu.h"

struct tile
{
    rect<olc::vf2d> bounds;
    short state = 0;
};

class TicTacToe : public scene
{
public:
    TicTacToe(int id, olcMFSG* game);
    bool Create() override;
    bool Update(float) override;

private:
    int turn;
    tile board[9];
    PauseMenu m_pausemenu;
    bool bGameOver;

    void checkGameOver();
};