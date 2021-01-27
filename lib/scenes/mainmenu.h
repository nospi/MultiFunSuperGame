#include "scene.h"
#include "button.h"
#include "rect.h"
#include <vector>

class MainMenu : public scene
{
public:
    MainMenu(int id, olcMFSG* game) : scene(id, "Multi Fun Super Game!", game) { }

    bool Create() override
    {
        for (int i = 1; i < game->GetScenes()->size(); i++)
        {
            scene* scn = game->GetScenes()->at(i);
            olc::vi2d optionsTopCenter { game->ScreenWidth() / 2, game->ScreenHeight() / 4 };
            olc::vi2d pos = optionsTopCenter + olc::vi2d(-225, (i - 1) * 60);
            rect<olc::vi2d> bounds(pos, { 450, 50 });
            vButtons.push_back(CreateLoadSceneButton(scn, bounds, game));
        }
        return true;
    }

    bool Update(float fElapsedTime)
    {
        if (game->GetMouse(0).bPressed)
            for (auto& b : vButtons)
                if (b.bounds.contains(game->GetMousePos()))
                {
                    b.onClick();
                    break;
                }

        game->Clear(olc::VERY_DARK_GREEN);
        
        // menu title - "Multi Fun Super Game!"
        unsigned txtSize = 5U;
        olc::vi2d txtPos{ game->ScreenWidth() / 2, 60 + 4 * (int)txtSize };
        txtPos.x -= ((int)title.length() * 8 * txtSize) / 2;
        game->DrawShadowedString(txtPos, olc::vi2d(3, 3), title, olc::WHITE, olc::BLACK, txtSize);

        // menu options
        for (auto& b : vButtons)
            b.DrawSelf(game);

        return !(game->GetKey(olc::ESCAPE).bPressed);
    }

private:
    std::vector<Button> vButtons;
};