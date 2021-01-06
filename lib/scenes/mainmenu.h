#include "scene.h"

class MainMenu : public scene
{
public:
    MainMenu(int id, olcMFSG* game) : scene(id, "Main Menu", game) { }

    bool Create() override
    {
        return true;
    }

    bool Update(float fElapsedTime)
    {
        // load scenes by ID (1-9 for now - TODO: rethink this)
        if (game->GetKey(olc::K1).bPressed)
            game->LoadScene(1);
        if (game->GetKey(olc::K2).bPressed)
            game->LoadScene(2);
        if (game->GetKey(olc::K3).bPressed)
            game->LoadScene(3);
        if (game->GetKey(olc::K4).bPressed)
            game->LoadScene(4);
        if (game->GetKey(olc::K5).bPressed)
            game->LoadScene(5);
        if (game->GetKey(olc::K6).bPressed)
            game->LoadScene(6);
        if (game->GetKey(olc::K7).bPressed)
            game->LoadScene(7);
        if (game->GetKey(olc::K8).bPressed)
            game->LoadScene(8);
        if (game->GetKey(olc::K9).bPressed)
            game->LoadScene(9);

        // begin drawing menu
        game->Clear(olc::VERY_DARK_GREEN);
        
        // menu title - "Main Menu"
        unsigned txtSize = 5U;
        olc::vi2d txtPos{ game->ScreenWidth() / 2, 30 + 4 * (int)txtSize };
        txtPos.x -= ((int)title.length() * 8 * txtSize) / 2;
        game->DrawShadowedString(txtPos, olc::vi2d(3, 3), "Main Menu", olc::WHITE, olc::BLACK, txtSize);

        // menu options
        olc::vi2d optionsTopCenter { game->ScreenWidth() / 2, game->ScreenHeight() / 3 };
        unsigned optionTextSize = 3U;
        for (int i = 1; i < game->GetScenes()->size(); i++)
        {
            scene* scn = game->GetScenes()->at(i);
            std::string txt = std::to_string(scn->id) + ") " + scn->title;
            olc::vi2d pos = optionsTopCenter;
            pos.y += (i - 1) * 8 * (optionTextSize * 2);
            pos.x -= (txt.length() * 8 * optionTextSize) / 2;
            game->DrawShadowedString(pos, olc::vi2d(3, 2), txt, olc::WHITE, olc::BLACK, optionTextSize);
        }

        return !(game->GetKey(olc::ESCAPE).bPressed);
    }
};