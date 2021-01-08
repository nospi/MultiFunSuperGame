#include "scenes/matrix.h"

struct sColumn
{
    int id;
    int length;
    float x;
    float y;
    float speed;
    uint32_t scale;

    sColumn(int id) : id(id)
    {
        length = rand() % 32 + 16;
        x = (float)id * 8.0f;
        speed = rand() % (int)(Matrix::MaxSpeed * 0.75f) + Matrix::MaxSpeed * 0.25f;
        
        scale = 1U;
        if (speed >= 0.87f * Matrix::MaxSpeed)
            scale = 2U;
        if (speed >= 0.98f * Matrix::MaxSpeed)
            scale = 3U;

        y = -(length * 8.0f * (float)scale);
    }

    bool operator==(const sColumn& other)
    {
        return id == other.id;
    }
};

float Matrix::MaxSpeed = 180.0f;

Matrix::Matrix(int id, olcMFSG* game) : scene(id, "Matrix Rain", game)
{
    mAlphabet = {
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
        'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
        'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
        'Y', 'Z', '0', '1', '2', '3', '4', '5',
        '6', '7', '8', '9', '!', '@', '#', '$',
        '%', '^', '&', '*'
    };

    mSpeedColours = {
        olc::WHITE,
        olc::GREEN,
        olc::DARK_GREEN,
        olc::VERY_DARK_GREEN,
        olc::VERY_DARK_GREY
    };
}

bool Matrix::Create()
{
    srand(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    mCols = game->ScreenWidth() / 8;
    mRows = game->ScreenWidth() / 8;
    mGrid = new char[mCols * mRows];
    for (int i = 0; i < mCols * mRows; i++)
        mGrid[i] = mAlphabet[rand() % mAlphabet.size()];
    for (int i = 0; i < mCols; i++)
        mColumns.emplace_back(i);
    std::sort(mColumns.begin(), mColumns.end(), [](auto a, auto b){ return a.speed < b.speed; });
    return true;
}

bool Matrix::Update(float fElapsedTime)
{
    std::vector<int> toRemove;

    for (auto& col : mColumns)
    {
        col.y += col.speed * fElapsedTime;
        if (col.y + col.length * col.scale > (float)game->ScreenHeight())
            toRemove.push_back(col.id);
    }

    for (auto& id : toRemove)
    {
        mColumns.erase(std::remove_if(mColumns.begin(), mColumns.end(), [id](auto col){ return id == col.id; }));
        mColumns.emplace_back(id);
    }
    
    if (toRemove.size())
        std::sort(mColumns.begin(), mColumns.end(), [](auto a, auto b){ return a.speed < b.speed; });
    
    for (auto& col : mColumns)
    {
        if (rand() % 10 == 0)
        {
            int x = rand() % mCols;
            int y = rand() % mRows;
            mGrid[y * mCols + x] = mAlphabet[rand() % mAlphabet.size()];
        }
    }

    // begin drawing routines

    game->Clear(0);

    for (auto& col : mColumns)
    {
        for (int i = 0; i < col.length; i++)
        {
            int offset = 8 * col.scale;
            int sy = floorf(col.y / offset) * offset + i * offset;
            if (sy < 0 || sy >= game->ScreenHeight())
                continue;
            int sx = (int)col.x;

            olc::Pixel p;
            if (i == col.length - 1)
            {
                p = mSpeedColours[0];
                p.a = (int)(col.speed / MaxSpeed * 240);
            }
            else if (col.speed < 0.5f * MaxSpeed)
                p = mSpeedColours[4];
            else if (col.speed < 0.8f * MaxSpeed)
                p = mSpeedColours[3];
            else if (col.speed < 0.9f * MaxSpeed)
                p = mSpeedColours[2];
            else
                p = mSpeedColours[1];
            
            if ((float)i < 0.25f * (float)col.length)
                p.a = (float)i / (0.25f * (float)col.length) * 255;

            std::string s;
            s += mGrid[(sy / 8) * mCols + (sx / 8)];
            game->DrawString(sx, sy, s, p, col.scale);
        }
    }

    return !(game->GetKey(olc::ESCAPE).bPressed);
}

bool Matrix::Destroy()
{
    if (mGrid != nullptr)
        delete[] mGrid;
    mColumns.clear();
    return true;
}