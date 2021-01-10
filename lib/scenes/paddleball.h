#pragma once

#include "olcMFSG.h"
#include "scene.h"
#include "rect.h"
#include "pausemenu.h"

struct sBall
{
    rect<olc::vf2d> bounds;
    olc::vf2d velocity;

    sBall()
    {
        bounds.size = { 25.0f, 25.0f };
    }

    void DrawSelf(olcMFSG* game)
    {
        game->FillRect(bounds.position, bounds.size, olc::WHITE);
    }
};

struct sPaddle
{
    int id;
    rect<olc::vf2d> bounds;
    float yVelocity;
    olc::Pixel colour;

    sPaddle()
    {
        bounds.size = { 25.0f, 100.0f };
        colour = olc::WHITE;
    }

    void DrawSelf(olcMFSG* game)
    { 
        game->FillRect(bounds.position, bounds.size, colour);
    }
};

class PaddleBall : public scene
{
public:
    PaddleBall(int id, olcMFSG* game);
    bool Create() override;
    bool Update(float) override;
    static float MaxPaddleVelocity;
    static float InputStrength;
    static float LinearDamping;
    static olc::vf2d InitialBallVelocity;

private:
    int m_nScoreP1;
    int m_nScoreP2;
    bool m_bGameOver;
    bool m_bActiveP1;
    bool m_bActiveP2;
    float m_fBallHoldTimer;
    rect<olc::vf2d> rTop;
    rect<olc::vf2d> rBot;
    sBall m_ball;
    sPaddle m_paddleP1;
    sPaddle m_paddleP2;
    PauseMenu m_pausemenu;

private:
    void ProcessBall(float);
    void ProcessActivePaddle(float, sPaddle*, float);
    void ProcessSimpleAI(float, sPaddle*);
    void ProcessAdvancedAI(float, sPaddle*);
    void Score(int nScoringPlayerID);
    void ResetBall(int nScoringPlayerID = 0, float holdTime = 1.5f);
};
