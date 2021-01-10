#include "scenes/paddleball.h"

// TODO - gameover conditions

float clamp(float input, float lo, float hi)
{
    if (input < lo) return lo;
    if (input > hi) return hi;
    return input;
}

float PaddleBall::MaxPaddleVelocity = 400.0f;
float PaddleBall::InputStrength = 15.0f;
float PaddleBall::LinearDamping = 5.0f;
olc::vf2d PaddleBall::InitialBallVelocity = { -1.75f, 0.0f };

PaddleBall::PaddleBall(int id, olcMFSG* game) : scene(id, "PaddleBall", game)
{
    // initialise pause menu
    m_pausemenu.Create(this, game);
    m_pausemenu.onRestart = [&]() { Create(); };
}

bool PaddleBall::Create()
{
    // initialise game state
    m_nScoreP1 = 0;
    m_nScoreP2 = 0;
    m_bGameOver = false;
    m_bActiveP1 = true;
    m_bActiveP2 = false;

    // initialise boundaries
    rTop.size = { (float)game->ScreenWidth(), 50.0f };
    rBot.size = rTop.size;
    rTop.position = { 0.0f, -rTop.size.y };
    rBot.position = { 0.0f, (float)game->ScreenHeight() };

    // initialise paddles
    m_paddleP1.id = 0;
    m_paddleP1.bounds.position = olc::vf2d(50.0f, (float)game->ScreenHeight() * 0.5f) - m_paddleP1.bounds.size * 0.5f;
    m_paddleP1.yVelocity = 0.0f;
    m_paddleP1.colour = olc::BLUE;
    m_paddleP2.id = 1;
    m_paddleP2.bounds.position = olc::vf2d((float)game->ScreenWidth() - 50.0f, (float)game->ScreenHeight() * 0.5f) - m_paddleP2.bounds.size * 0.5f;
    m_paddleP2.yVelocity = 0.0f;
    m_paddleP2.colour = olc::RED;

    // initialise ball
    ResetBall();

    return true;
}

bool PaddleBall::Update(float fElapsedTime)
{
    if (game->GetKey(olc::ESCAPE).bPressed)
        m_pausemenu.Toggle();
    
    if (game->GetKey(olc::F1).bPressed)
        m_bActiveP1 = !m_bActiveP1;
    if (game->GetKey(olc::F2).bPressed)
        m_bActiveP2 = !m_bActiveP2;

    if (!m_pausemenu.Enabled())
    {
        // paddle 1
        if (m_bActiveP1)
        {
            // handle p1 input
            float input = 0.0f;
            if (game->GetKey(olc::W).bHeld) input -= InputStrength;
            if (game->GetKey(olc::S).bHeld) input += InputStrength;
            ProcessActivePaddle(fElapsedTime, &m_paddleP1, input);
        }
        else
        {
            // handle p1 ai
            ProcessSimpleAI(fElapsedTime, &m_paddleP1);
        }

        // paddle 2
        if (m_bActiveP2)
        {
            // handle p2 input
            float input = 0.0f;
            if (game->GetKey(olc::UP).bHeld) input -= InputStrength;
            if (game->GetKey(olc::DOWN).bHeld) input += InputStrength;
            ProcessActivePaddle(fElapsedTime, &m_paddleP2, input);
        }
        else
        {
            // handle p2 ai
            ProcessSimpleAI(fElapsedTime, &m_paddleP2);
        }

        ProcessBall(fElapsedTime);
    }

    // begin draw routines
    game->Clear(olc::VERY_DARK_GREY);

    // center line
    game->DrawLine({ game->ScreenWidth() / 2, 0 }, { game->ScreenWidth() / 2, game->ScreenHeight() }, olc::DARK_GREY, 0x00FFFF00);

    // game entities
    m_paddleP1.DrawSelf(game);
    m_paddleP2.DrawSelf(game);
    m_ball.DrawSelf(game);

    // ui
    olc::vi2d vpos { game->ScreenWidth() / 2 - 32 - 30, 30 };
    game->DrawShadowedString(vpos, { 2, 2 }, std::to_string(m_nScoreP1), olc::WHITE, olc::BLACK, 4U);
    vpos.x = game->ScreenWidth() / 2 + 30;
    game->DrawShadowedString(vpos, { 2, 2 }, std::to_string(m_nScoreP2), olc::WHITE, olc::BLACK, 4U);

    if (!m_bActiveP1)
    {
        std::string txt = "Press F1 to join";
        uint32_t size = 2U;
        olc::vi2d pos = { game->ScreenWidth() / 4, game->ScreenHeight() - 20 };
        pos.x -= txt.length() * 4 * size;
        pos.y -= 4 * size;
        game->DrawShadowedString(pos, { 2, 2 }, txt, olc::DARK_YELLOW, olc::BLACK, size);
    }

    if (!m_bActiveP2)
    {
        std::string txt = "Press F2 to join";
        uint32_t size = 2U;
        olc::vi2d pos = { 3 * (game->ScreenWidth() / 4), game->ScreenHeight() - 20 };
        pos.x -= txt.length() * 4 * size;
        pos.y -= 4 * size;
        game->DrawShadowedString(pos, { 2, 2 }, txt, olc::DARK_YELLOW, olc::BLACK, size);
    }

    return m_pausemenu.Process(fElapsedTime);
}

void PaddleBall::ProcessBall(float fElapsedTime)
{
    // hold ball after reset
    if (m_fBallHoldTimer > 0.0f)
    {
        m_fBallHoldTimer -= fElapsedTime;
        return;
    }

    // create temp rect
    rect<olc::vf2d> rNext;
    rNext.size = m_ball.bounds.size;
    rNext.position = m_ball.bounds.position + m_ball.velocity;
    
    // check if move would result in collision
    if (rNext.collides(m_paddleP1.bounds))
    {
        m_ball.velocity.x *= -1.0f;
        m_ball.velocity.y += m_paddleP1.yVelocity * 0.0025f;
    }
    if (rNext.collides(m_paddleP2.bounds))
    {
        m_ball.velocity.x *= -1.0f;
        m_ball.velocity.y += m_paddleP2.yVelocity * 0.0025f;
    }
    if (rNext.collides(rTop) || rNext.collides(rBot))
        m_ball.velocity.y *= -1.0f;

    // move
    m_ball.bounds.position += m_ball.velocity;

    // check scoring collision
    if (m_ball.bounds.position.x > game->ScreenWidth())
        Score(0);
    if (m_ball.bounds.position.x < -m_ball.bounds.size.x)
        Score(1);
}

void PaddleBall::ProcessActivePaddle(float fElapsedTime, sPaddle* paddle, float inputVelocity)
{
    // linear damping
    paddle->yVelocity -= paddle->yVelocity * LinearDamping * fElapsedTime;

    // acceleration -> velocity
    paddle->yVelocity += inputVelocity;
    paddle->yVelocity = clamp(paddle->yVelocity, -MaxPaddleVelocity, MaxPaddleVelocity);

    // velocity -> position (check)
    rect<olc::vf2d> rNext;
    rNext.size = paddle->bounds.size;
    rNext.position = paddle->bounds.position + olc::vf2d(0.0f, paddle->yVelocity) * fElapsedTime;

    // reflect if colliding with top/bottom
    if (rNext.collides(rTop) || rNext.collides(rBot))
        paddle->yVelocity *= -1.0f;
    
    // post-collision velocity -> position (execute)
    paddle->bounds.position.y += paddle->yVelocity * fElapsedTime;
}

void PaddleBall::ProcessSimpleAI(float fElapsedTime, sPaddle* paddle)
{
    float deltaY = m_ball.bounds.position.y - paddle->bounds.position.y - paddle->bounds.size.y * 0.5f;
    deltaY = clamp(deltaY, -InputStrength, InputStrength);

    ProcessActivePaddle(fElapsedTime, paddle, deltaY);
    return;
}

void PaddleBall::ProcessAdvancedAI(float fElapsedTime, sPaddle* paddle)
{
    return;
}

void PaddleBall::Score(int nScoringPlayerID)
{
    switch(nScoringPlayerID)
    {
    case 0: m_nScoreP1++; break;
    case 1: m_nScoreP2++; break;
    }
    ResetBall(nScoringPlayerID);
}

void PaddleBall::ResetBall(int nScoringPlayerID, float holdTime)
{
    m_ball.bounds.position = olc::vf2d((float)game->ScreenWidth() * 0.5f, (float)game->ScreenHeight() * 0.5f) - m_ball.bounds.size * 0.5f;
    m_ball.velocity = InitialBallVelocity;
    m_ball.velocity.y = (float)rand() / (float)RAND_MAX * 0.4f - 0.2f;
    if (nScoringPlayerID == 1)
        m_ball.velocity.x *= -1.0f;
    m_fBallHoldTimer = holdTime;
}