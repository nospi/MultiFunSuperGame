#include "scenes/spaceguns.h"


// ==   asteroid    =================================================================================================

asteroid::asteroid()
{
    core.radius = (float)rand() / (float)RAND_MAX * 50.0f + 12.5f;
    for (int i = 0; i < 8; i++)
        points.emplace_back((float)rand() / (float)RAND_MAX * 6.0f - 3.0f, (float)rand() / (float)RAND_MAX * 6.0f - 3.0f);
}

void asteroid::Update(float fElapsedTime, olcMFSG* game)
{
    core.center += velocity * fElapsedTime;
    rotation += ang_velocity * fElapsedTime;
    game->WrapCoordinates(core.center);
    if (health <= 0) removable = true;
}

void asteroid::DrawSelf(olcMFSG* game)
{
    float cs = cos(rotation);
    float sn = sin(rotation);

    auto getModulatedPoint = [&, sn, cs](int index)
    {
        // determine point on circle and modulate
        float step = (2.0f * M_PI) / (float)points.size();
        float theta = std::fmod(index * step, 2.0f * M_PI);
        auto vPos = olc::vf2d(core.radius * cos(theta), core.radius * sin(theta)); 
        vPos += points[index % points.size()];

        // rotate
        olc::vf2d tmp;
        tmp.x = vPos.x * cs - vPos.y * sn;
        tmp.y = vPos.x * sn + vPos.y * cs;
        vPos = tmp;

        // add to core position
        return core.center + vPos;
    };

    game->FillCircle(core.center, core.radius, olc::BLACK);

    for (int i = 0; i < points.size(); i++)
    {
        auto p1 = getModulatedPoint(i);
        auto p2 = getModulatedPoint(i + 1);
        game->DrawLine(p1, p2, olc::GREEN);
    }
    return;
}


// ==   projectile    ===============================================================================================

projectile::projectile(const olc::vf2d& position, const olc::vf2d& velocity, olcMFSG* game) 
    : body(position, 1.0f), velocity(velocity) 
{
    this->game = game;
}

void projectile::Update(float fElapsedTime)
{
    body.center += velocity * fElapsedTime;
    
    if (body.center.x < 0.0f || 
        body.center.x >= (float)game->ScreenWidth() ||
        body.center.y < 0.0f || 
        body.center.y >= (float)game->ScreenWidth())
    {
        removable = true;
    }
}

void projectile::DrawSelf()
{
    game->FillCircle(body.center, body.radius);
}


// ==   spacecraft    ===============================================================================================

spacecraft::spacecraft()
{
    points = {
        { -5.0f, -5.0f },
        { 5.0f, -5.0f },
        { 0.0f, 10.0f }
    };
}

void spacecraft::Create(olcMFSG* game, SpaceGuns* scene)
{
    this->game = game;
    this->scene = scene;
    shootCooldown = 0.0f;
}

void spacecraft::Update(float fElapsedTime)
{
    if (shootCooldown > 0.0f)
        shootCooldown -= fElapsedTime;

    if (game->GetKey(olc::SPACE).bHeld && shootCooldown <= 0.0f)
    {
        olc::vf2d pVel;
        pVel.x = cos(deg2rad(transform.rotation));
        pVel.y = sin(deg2rad(transform.rotation));
        scene->SpawnProjectile(transform.position + pVel.norm() * 5.0f, pVel.norm() * 300.0f);
        shootCooldown = 0.33f;
    }

    // friction (for now)
    // velocity -= velocity * 0.5f * fElapsedTime;

    // rotation
    if (game->GetKey(olc::A).bHeld)
        transform.rotation -= .07f * fElapsedTime;
    if (game->GetKey(olc::D).bHeld)
        transform.rotation += .07f * fElapsedTime;

    // thrust
    if (game->GetKey(olc::W).bHeld)
    {
        olc::vf2d forward;
        forward.x = cos(deg2rad(transform.rotation));
        forward.y = sin(deg2rad(transform.rotation));
        velocity += forward.norm();
    }

    // limit velocity
    if (velocity.mag() > 200.0f)
        velocity = velocity.norm() * 200.0f;

    // apply velocity
    transform.position += velocity * fElapsedTime;

    // wrap around screen edges
    if (transform.position.x < 0.0f)
        transform.position.x = (float)game->ScreenWidth() - 1.0f;
    if (transform.position.x >= (float)game->ScreenWidth())
        transform.position.x = 0.0f;

    if (transform.position.y < 0.0f)
        transform.position.y = (float)game->ScreenHeight() - 1.0f;
    if (transform.position.y >= (float)game->ScreenHeight())
        transform.position.y = 0.0f;
}

void spacecraft::DrawSelf()
{
    float sn = sin(deg2rad(transform.rotation) - M_PI * 0.5f);
    float cs = cos(deg2rad(transform.rotation) - M_PI * 0.5f);

    auto rotate = [cs, sn](const olc::vf2d& p)
    {
        olc::vf2d tmp;
        tmp.x = p.x * cs - p.y * sn;
        tmp.y = p.x * sn + p.y * cs;
        return tmp;
    };

    for (int i = 0; i < points.size(); i++)
    {
        auto p1 = transform.position + rotate(points[i]);
        auto p2 = transform.position + rotate(points[(i + 1) % points.size()]);
        game->DrawLine(p1, p2, bIsColliding ? olc::RED : olc::YELLOW);
    }
}


// ==   starfield   =============================================================================================

starfield::starfield() {}

void starfield::Create(olcMFSG* game)
{
    stars.clear();
    this->game = game;
    for (int i = 0; i < 300; i++)
        stars.emplace_back(olc::vf2d((float)(rand() % game->ScreenWidth()), (float)(rand() % game->ScreenHeight())), (float)rand() / (float)RAND_MAX * 0.5f);
    velocity = { (float)rand() / (float)RAND_MAX * 3.0f - 1.5f, (float)rand() / (float)RAND_MAX * 3.0f - 1.5f };
}

void starfield::Update(float fElapsedTime)
{
    std::for_each(stars.begin(), stars.end(), [&, fElapsedTime](circle& s){ 
        s.center += velocity * fElapsedTime; 
        game->WrapCoordinates(s.center);
    });
}

void starfield::DrawSelf()
{
    olc::Pixel p = olc::WHITE;
    for (auto& s : stars)
    {
        p.a = rand() % 55 + 200;
        game->FillCircle(s.center, s.radius, p);
    }
}


// ==   spaceguns   =============================================================================================

SpaceGuns::SpaceGuns(int id, olcMFSG* game) : scene(id, "SpaceGuns", game)
{
    // initialise pause menu
    m_pausemenu.Create(this, game);
    m_pausemenu.onRestart = [&](){ Create(); };
}

bool SpaceGuns::Create()
{
    // set wrap-coords flag in game
    game->bScreenWrapEnabled = true;

    // initialise game state
    m_nScore = 0;
    m_fTime = 0.0f;
    m_fSpawnTime = 7.5f;
    m_bGameOver = false;
    m_vAsteroids.clear();
    m_vProjectiles.clear();
    
    // initialise player state
    m_Player.Create(game, this);
    m_Player.transform.position = { (float)game->ScreenWidth() * 0.5f, (float)game->ScreenHeight() * 0.5f };
    m_Player.velocity = { 0.0f, 0.0f };
    m_Player.health = 100.0f;

    // initialise asteroids vector
    for (int i = 0; i < 4; i++)
    {
        m_vAsteroids.emplace_back();
        m_vAsteroids[i].core.center = { (float)(rand() % game->ScreenWidth()), (float)(rand() % game->ScreenHeight()) };
        m_vAsteroids[i].ang_velocity = (float)rand() / (float)RAND_MAX - 0.5f;
        m_vAsteroids[i].velocity = { (float)rand() / (float)RAND_MAX * 50.0f - 25.0f, (float)rand() / (float)RAND_MAX * 50.0f - 25.0f };
    }

    // initialise background starfield
    m_Starfield.Create(game);

    return true;
}

bool SpaceGuns::Destroy()
{
    // unset wrap-coords flag in game
    game->bScreenWrapEnabled = false;
    return true;
}

bool SpaceGuns::Update(float fElapsedTime)
{
    if (m_bGameOver)
    {
        m_pausemenu.Show();
    }

    if (game->GetKey(olc::ESCAPE).bPressed)
        m_pausemenu.Toggle();

    if (!m_pausemenu.Enabled())
    {
        // update starfield
        m_Starfield.Update(fElapsedTime);

        // spawn asteroids on timer
        m_fTime += fElapsedTime;
        if (m_fTime >= m_fSpawnTime)
        {
            m_fTime -= m_fSpawnTime;
            m_fSpawnTime -= m_fSpawnTime * 0.05f;
            if (m_fSpawnTime <= 3.0f)
                m_fSpawnTime = 3.0f;

            olc::vf2d pos, vel;
            pos.x = (float)(rand() % game->ScreenWidth());
            pos.y = (float)(rand() % game->ScreenHeight());
            vel.x = (float)rand() / (float)RAND_MAX * 100.0f - 50.0f;
            vel.y = (float)rand() / (float)RAND_MAX * 100.0f - 50.0f;
            float ang_vel = (float)rand() / (float)RAND_MAX * 2.0f * M_PI - M_PI;
            SpawnAsteroid(pos, vel, ang_vel);
        }

        m_Player.Update(fElapsedTime);
        if (m_Player.health <= 0.0f)
        {
            m_Player.health = 0.0f;
            m_bGameOver = true;
        }            
        
        bool colliding = false;
        for (auto& ast : m_vAsteroids)
        {
            ast.Update(fElapsedTime, game);
            if (ast.core.contains(m_Player.transform.position))
            {
                m_Player.health -= 100.0f * fElapsedTime;
                colliding = true;
            }                
        }

        m_Player.bIsColliding = colliding;

        for (auto& p : m_vProjectiles)
        {
            p.Update(fElapsedTime);
            for (auto& a : m_vAsteroids)
                if (a.core.collides(p.body))
                {
                    p.removable = true;
                    a.health--;
                    a.core.radius *= 0.75f;
                    m_nScore += (3 - a.health) * 50;
                }
        }
        
        m_vAsteroids.erase(std::remove_if(
                m_vAsteroids.begin(), 
                m_vAsteroids.end(), 
                [](const asteroid a){ return a.removable; }), 
            m_vAsteroids.end());
        
        m_vProjectiles.erase(std::remove_if(
                m_vProjectiles.begin(), 
                m_vProjectiles.end(), 
                [](const projectile p){ return p.removable; }), 
            m_vProjectiles.end());
    }

    game->Clear(0);

    m_Starfield.DrawSelf();
    
    for (auto& ast: m_vAsteroids)
        ast.DrawSelf(game);

    m_Player.DrawSelf();

    for (auto& p : m_vProjectiles)
        p.DrawSelf();
    
    game->DrawShadowedString({ 16, 16 }, { 2, 2 }, "Score: " + std::to_string(m_nScore), olc::WHITE, olc::VERY_DARK_GREY, 2U);
    game->DrawShadowedString({ 16, 40 }, { 2, 2 }, "Health: " + std::to_string(m_Player.health), olc::WHITE, olc::VERY_DARK_GREY, 2U);

    return m_pausemenu.Process(fElapsedTime);
}

void SpaceGuns::SpawnAsteroid(const olc::vf2d& position, const olc::vf2d& velocity, float ang_velocity)
{
    asteroid a;
    a.core.center = position;
    a.velocity = velocity;
    a.ang_velocity = ang_velocity;
    a.health = 4;
    a.removable = false;
    m_vAsteroids.push_back(a);
}

void SpaceGuns::SpawnProjectile(const olc::vf2d& position, const olc::vf2d& velocity)
{
    m_vProjectiles.emplace_back(position, velocity, game);
}