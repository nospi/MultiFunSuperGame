#include "scenes/minesweeper.h"



using cell = MineSweeper::cell;

cell::cell() {}

cell::cell(int x, int y) : position(x, y) {}

MineSweeper::MineSweeper(int id, olcMFSG* game) : scene(id, "MineSweeper", game)
{
    mPauseMenu.Create(this, game);
    mPauseMenu.onRestart = [&]() { Create(); };
}

bool MineSweeper::Create()
{
    bGameOver = false;
    bFirstClick = true;
    nCols = game->ScreenWidth() / 64;
    nRows = game->ScreenHeight() / 64;
    nMines = (nCols * nRows) / 3;
    vCellSize = { game->ScreenWidth() / nCols, game->ScreenHeight() / nRows };
    vCells.clear();
    for (int y = 0; y < nRows; y++)
        for (int x = 0; x < nCols; x++)
            vCells.emplace_back(x, y);
    return true;
}

bool MineSweeper::Update(float fElapsedTime)
{
    if (game->GetKey(olc::ESCAPE).bPressed)
        mPauseMenu.Toggle();

    if (bGameOver)
        mPauseMenu.Show();

    if (!mPauseMenu.Enabled())
    {
        olc::vi2d vCursor = game->GetMousePos() / vCellSize;
        int idx = vCursor.y * nCols + vCursor.x;

        // select cell on lmb
        if (game->GetMouse(0).bPressed && vCells.at(idx).enabled && !vCells.at(idx).flagged)
        {
            if (bFirstClick)
            {
                // plant mines
                bFirstClick = false;
                int mines = nMines;
                while (mines)
                {
                    olc::vi2d pos = { rand() % nCols, rand() % nRows };
                    if (pos != vCursor && !vCells.at(pos.y * nCols + pos.x).explosive)
                    {
                        vCells.at(pos.y * nCols + pos.x).explosive = true;
                        mines--;
                    }
                }
                for (auto& c : vCells)
                    calculateValue(c);
            }

            // flip cell
            clickCell(vCells.at(idx));
        }
        
        // toggle danger flag on rmb
        if (game->GetMouse(1).bPressed && vCells.at(idx).enabled)
            vCells.at(idx).flagged = !vCells.at(idx).flagged;
    }

    game->Clear(0);

    for (auto& c : vCells)
        drawCell(c);
    
    if (bGameOver)
    {
        game->SetPixelMode(olc::Pixel::ALPHA);
        game->FillRect(0, 0, game->ScreenWidth(), game->ScreenHeight(), { 0, 0, 0, 127 });
        game->SetPixelMode(olc::Pixel::NORMAL);

        std::string sText = "Game Over!";
        if (nSolved == nRows * nCols - nMines) sText = "You Win!";
        int scale = 4;
        olc::vi2d pos = { game->ScreenWidth() / 2, game->ScreenHeight() / 4 };
        pos -= { (int)sText.length() * scale * 4, scale * 4 };
        game->DrawShadowedString(pos, { 3, 3 }, sText, olc::WHITE, olc::BLACK, scale);
    }

    return mPauseMenu.Process(fElapsedTime);
}

void MineSweeper::drawCell(const cell& c)
{
    olc::vi2d border = { 2, 2 };
    olc::Pixel colour;
    if (c.enabled)
    {
        colour = c.flagged ? olc::CYAN : olc::GREY;
        game->FillRect(c.position * vCellSize + border, vCellSize - border * 2, colour);
    }
    else
    {
        colour = c.explosive ? olc::RED : olc::VERY_DARK_GREY;
        game->FillRect(c.position * vCellSize + border, vCellSize - border * 2, colour);

        if (!c.explosive && c.value != 0)
        {
            std::string label = std::to_string(c.value);
            int scale = 2;
            olc::vi2d pos = c.position * vCellSize + vCellSize / 2;
            olc::vi2d offset = olc::vi2d((int)label.length() * scale, scale) * 4;
            game->DrawShadowedString(pos - offset, { 2, 2 }, label, olc::WHITE, olc::BLACK, scale);
        }
    }
}

void MineSweeper::clickCell(cell& c)
{
    if (c.explosive)
    {
        bGameOver = true;
        for (auto& c : vCells)
            c.enabled = false;
        return;
    }

    c.enabled = false;
    nSolved++;
    if (nSolved == nRows * nCols - nMines)
        bGameOver = true;

    if (c.value == 0)
        for (int dy = -1; dy < 2; dy++)
            for (int dx = -1; dx < 2; dx++)
            {
                int x = c.position.x + dx;
                int y = c.position.y + dy;
                if (dx == 0 && dy == 0) continue;
                if (x < 0 || x >= nCols || y < 0 || y >= nCols) continue;

                if (vCells.at(y * nCols + x).enabled)
                    clickCell(vCells.at(y * nCols + x));
            }
}

void MineSweeper::calculateValue(cell& c)
{
    int value = 0;
    for (int dy = -1; dy < 2; dy++)
        for (int dx = -1; dx < 2; dx++)
        {
            int x = c.position.x + dx;
            int y = c.position.y + dy;
            if (x < 0 || x >= nCols || y < 0 || y >= nRows) continue;
            if (dx == 0 && dy == 0) continue;
            if (vCells[y * nCols + x].explosive)
                value++;
        }
    c.value = value;
}