#include "scenes/tictactoe.h"

TicTacToe::TicTacToe(int id, olcMFSG* game) : scene(id, "TicTacToe", game)
{
    m_pausemenu.Create(this, game);
    m_pausemenu.onRestart = [&](){ Create(); };
}

bool TicTacToe::Create()
{
    olc::vf2d scnCenter = olc::vf2d((float)game->ScreenWidth() * 0.5f, (float)game->ScreenHeight() * 0.5f);
    float gutter = (float)game->ScreenHeight() * 0.25f / 4.0f;
    for (int y = 0; y < 3; y++)
        for (int x = 0; x < 3; x++)
        {
            tile t;
            t.bounds.size = olc::vf2d((float)game->ScreenHeight(), (float)game->ScreenHeight()) * 0.25f;
            t.bounds.position = scnCenter + olc::vf2d(x - 1.0f, y - 1.0f) * (t.bounds.size + olc::vf2d(gutter, gutter)) - t.bounds.size * 0.5f;
            t.state = 0;
            board[y * 3 + x] = t;
        }   
    turn = 0;
    bGameOver = false;
    return true;
}

bool TicTacToe::Update(float fElapsedTime)
{
    if (game->GetKey(olc::ESCAPE).bPressed)
        m_pausemenu.Toggle();

    if (!bGameOver && game->GetMouse(0).bPressed)
    {
        for (auto& tile : board)
        {
            if (tile.state == 0 && tile.bounds.contains((olc::vf2d)game->GetMousePos()))
            {
                tile.state = turn % 2 + 1;
                turn++;
                checkGameOver();
                break;
            }
        }
    }

    game->Clear(0);

    for (int i = 0; i < 9; i++)
    {
        olc::Pixel p;
        switch (board[i].state)
        {
        case 0: p = olc::DARK_GREY; break;
        case 1: p = olc::BLUE; break;
        case 2: p = olc::RED; break;
        }
        if (bGameOver) p /= 3;
        game->FillRect(board[i].bounds.topLeft(), board[i].bounds.size, p);
    }

    if (bGameOver)
    {
        std::string sGameOver = "Game Over!";
        std::string sWinner = "Player " + std::to_string((turn - 1) % 2 + 1) + " wins!";
        olc::vf2d pos = { (float)game->ScreenWidth() * 0.5f - sGameOver.size() * 4 * 4, (float)game->ScreenHeight() * 0.2f - 4 * 4 };
        game->DrawShadowedString(pos, { 2, 2 }, sGameOver, olc::RED, olc::VERY_DARK_YELLOW, 4U);
        pos.x = (float)game->ScreenWidth() * 0.5f - sWinner.size() * 4 * 2;
        pos.y += 50.0f;
        game->DrawShadowedString(pos, { 2, 2 }, sWinner, olc::WHITE, olc::VERY_DARK_GREEN, 2U);
        m_pausemenu.Show();
    }

    return m_pausemenu.Process(fElapsedTime);
}

void TicTacToe::checkGameOver()
{
    short combos[8][3] = {
        { 0, 1, 2 },
        { 3, 4, 5 },
        { 6, 7, 8 },
        { 0, 3, 6 },
        { 1, 4, 7 },
        { 2, 5, 8 },
        { 0, 4, 8 },
        { 6, 4, 2 }
    };

    for (int i = 0; i < 8; i++)
    {
        if (board[combos[i][0]].state == board[combos[i][1]].state)
            if (board[combos[i][1]].state == board[combos[i][2]].state)
            {
                if (board[combos[i][0]].state != 0)
                {
                    bGameOver = true;
                    return;
                }
            }
    }
}