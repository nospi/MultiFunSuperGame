#include "scene.h"

class Noise : public scene
{
public:
    static int PixelSize;

    Noise(int id, olcMFSG* game) : scene(id, "Noise", game) { }

    bool Create() override
    {
        return true;
    }

    bool Update(float fElapsedTime) override
    {
        if (game->GetKey(olc::NP_ADD).bPressed)
            PixelSize *= 2;
        if (game->GetKey(olc::NP_SUB).bPressed)
            PixelSize /= 2;
        
        // clamp pixel size
        if (PixelSize <= 0)
            PixelSize = 1;
        if (PixelSize > 256)
            PixelSize = 256;

        game->Clear(0);
        for (int y = 0; y < game->ScreenHeight() / PixelSize; y++)
            for (int x = 0; x < game->ScreenWidth() / PixelSize; x++)
            {
                olc::Pixel p;
                p.r = rand() % 255;
                p.g = rand() % 255;
                p.b = rand() % 255;
                p.a = 255;
                game->FillRect(x * PixelSize, y * PixelSize, PixelSize, PixelSize, p);
            }
        return !(game->GetKey(olc::ESCAPE).bPressed);
    }
};

int Noise::PixelSize = 4;