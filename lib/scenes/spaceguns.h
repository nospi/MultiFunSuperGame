#pragma once

#include "olcMFSG.h"
#include "scene.h"
#include "transform.h"
#include "circle.h"
#include "pausemenu.h"
#include <vector>

class asteroid
{
public:
    circle core;
    float rotation = 0.0f;
    olc::vf2d velocity;
    float ang_velocity;
    std::vector<olc::vf2d> points;
    bool removable = false;
    int health = 3;

    asteroid();
    void Update(float, olcMFSG* game);
    void DrawSelf(olcMFSG* game);
};

class projectile
{
public:
    circle body;
    olc::vf2d velocity;
    bool removable = false;
    olcMFSG* game = nullptr;

    projectile(const olc::vf2d&, const olc::vf2d&, olcMFSG* game);
    void Update(float);
    void DrawSelf();
};

class SpaceGuns;

class spacecraft
{
public:
    mfsg::transform transform;
    olc::vf2d velocity;
    std::vector<olc::vf2d> points;
    float health;
    float shootCooldown;
    olcMFSG* game;
    SpaceGuns* scene;
    bool bIsColliding;

    spacecraft();
    void Create(olcMFSG* game, SpaceGuns* scene);
    void Update(float);
    void DrawSelf();
};

class starfield
{
public:
    std::vector<circle> stars;
    olc::vf2d velocity;
    olcMFSG* game;

    starfield();
    void Create(olcMFSG*);
    void Update(float);
    void DrawSelf();
};

class SpaceGuns : public scene
{
public:
    SpaceGuns(int, olcMFSG*);
    bool Create() override;
    bool Update(float) override;
    bool Destroy() override;
    void SpawnAsteroid(const olc::vf2d& position, const olc::vf2d& velocity, float ang_velocity);
    void SpawnProjectile(const olc::vf2d& position, const olc::vf2d& velocity);

private:
    int m_nScore;
    float m_fTime;
    float m_fSpawnTime;
    bool m_bGameOver;
    spacecraft m_Player;
    starfield m_Starfield;
    std::vector<asteroid> m_vAsteroids;
    std::vector<projectile> m_vProjectiles;
    std::vector<circle> m_vStarfield;
    olc::vf2d m_vStarfieldVelocity;
    PauseMenu m_pausemenu;
};