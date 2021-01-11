#include "scenes/boids.h"

float manhattanDistance(const olc::vf2d& a, const olc::vf2d& b)
{
    return abs(a.x - b.x) + abs(a.y - b.y);
}

float distance(const olc::vf2d&a, const olc::vf2d& b)
{
    return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

float distanceSqd(const olc::vf2d&a, const olc::vf2d& b)
{
    return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y);
}

struct sBoid
{
    int id = -1;
    mfsg::transform transform;
    olc::vf2d velocity{0.0f, 0.0f};
    olc::Pixel colour = olc::YELLOW;
    bool isPerching;
    float perchTimer = 1.0f;

    sBoid() {}

    sBoid(int id) : id(id) {}

    void Process(float fElapsedTime, const std::vector<sBoid>& vBoids, olcMFSG* game)
    {
        // do nothing if boid is perched
        if (isPerching)
        {
            if (perchTimer > 0.0f)
                perchTimer -= fElapsedTime;
            else
                isPerching = false;
            return;
        }

        // determine neighbours within field of view
        float fMaxRadius = std::max(std::max(Boids::Radius_Separation, Boids::Radius_Cohesion), Boids::Radius_Alignment);
        std::vector<sBoid> neighbours;
        circle cFOV(transform.position, fMaxRadius);
        for (auto& b : vBoids)
        {
            if (cFOV.contains(b.transform.position))
                neighbours.push_back(b);
            if (neighbours.size() >= 10)
                break;
        }
    
        // flock to mouse
        olc::vf2d vToMouse = ((olc::vf2d)game->GetMousePos() - transform.position) / 100.0f * Boids::ForceStrength_Mouse;

        // boundary avoidance
        olc::vf2d vBounding{ 0.0f, 0.0f };
        float gutter = 40.0f;
        float boundaryAvoidance = 0.2f;
        if (transform.position.x <= gutter) vBounding.x = gutter * boundaryAvoidance;
        if (transform.position.x >= (float)game->ScreenWidth() - gutter) vBounding.x = -(gutter) * boundaryAvoidance;
        if (transform.position.y <= gutter) vBounding.y = gutter * boundaryAvoidance;
        if (transform.position.y >= (float)game->ScreenHeight() - gutter) vBounding.y = -(gutter) * boundaryAvoidance;

        // separation
        olc::vf2d vSeparation{ 0.0f, 0.0f };
        for (auto& b : vBoids)
            if (id != b.id)
                if (distanceSqd(transform.position, b.transform.position) < Boids::Radius_Separation * Boids::Radius_Separation)
                    vSeparation += (b.transform.position - transform.position) * Boids::ForceStrength_Separation;

        // cohesion
        olc::vf2d vCohesion{ 0.0f, 0.0f };
        if (neighbours.size() > 1)
        {
            for (auto& b : neighbours)
                if (id != b.id)
                    vCohesion += b.transform.position;
            vCohesion /= std::max(1, (int)neighbours.size() - 1);
            vCohesion = (vCohesion - transform.position) / 100.0f * Boids::ForceStrength_Cohesion;
        }

        // alignment
        olc::vf2d vAlignment{ 0.0f, 0.0f };
        for (auto& b : neighbours)
            if (id != b.id)
                vAlignment += b.velocity;
        vAlignment /= std::max(1, (int)neighbours.size() - 1);
        vAlignment = (vAlignment - velocity) / 100.0f * Boids::ForceStrength_Alignment;

        // perching
        float fGroundLevel = game->ScreenHeight() - 10.0f;
        if (!isPerching && transform.position.y >= fGroundLevel)
        {
            transform.position.y = fGroundLevel;
            isPerching = true;
            perchTimer = (float)rand() / (float)RAND_MAX * 0.3f;
        }

        // sum rules
        velocity += (vToMouse + vBounding + vSeparation + vCohesion + vAlignment) * fElapsedTime;

        // limit velocity
        if (velocity.mag() > Boids::MaxSpeed * fElapsedTime)
            velocity = velocity / velocity.mag() * Boids::MaxSpeed * fElapsedTime;
        
        // calculate rotation & colour
        transform.rotation = atan2f(velocity.y, velocity.x);
        colour.r = (int)(velocity.mag() / (Boids::MaxSpeed * fElapsedTime) * 255.0f);
        colour.b = 255 - (colour.r);

        // move
        transform.position += velocity;
    }
    
    void DrawSelf(olcMFSG* game)
    {
        olc::vf2d points[] = { 
            olc::vf2d(-2.0f, -10.0f) * transform.scale, 
            olc::vf2d(2.0f, -10.0f) * transform.scale, 
            olc::vf2d(0.0f, 0.0f) * transform.scale 
        };

        for (auto& pt : points)
        {
            olc::vf2d rot;
            float theta = transform.rotation - (90.0f * 3.14159f / 180.0f);
            float sn = sin(theta);
            float cs = cos(theta);
            rot.x = pt.x * cs - pt.y * sn;
            rot.y = pt.x * sn + pt.y * cs;
            pt = transform.position + rot;
        }

        if (id != 0)
            colour.a = 127;

        game->FillTriangle(points[0], points[1], points[2], colour);
    }
};

int Boids::MaxBoidCount = 300;
float Boids::MaxSpeed = 550.0f;
float Boids::ForceStrength_Mouse = 0.75f;
float Boids::ForceStrength_Alignment = 0.15f;
float Boids::ForceStrength_Cohesion = 2.0f;
float Boids::ForceStrength_Separation = 0.001f;
float Boids::Radius_Separation = 20.0f;
float Boids::Radius_Cohesion = 60.0f;
float Boids::Radius_Alignment = 25.0f;

Boids::Boids(int id, olcMFSG* game) : scene(id, "Boids", game) {}

bool Boids::Create()
{
    for (int i = 0; i < MaxBoidCount; i++)
    {
        mBoids.emplace_back(i);
        mBoids[i].transform.position = { (float)(rand() % game->ScreenWidth()), (float)(rand() % game->ScreenHeight()) };
        mBoids[i].transform.scale = (float)rand() / (float)RAND_MAX * 0.5f + 0.75f;
    }
    return true;
}

bool Boids::Update(float fElapsedTime)
{
    for (auto& b : mBoids)
        b.Process(fElapsedTime, mBoids, game);
    
    // for (auto & b : mBoids)
    //     b.transform.position += b.velocity;

    game->Clear(0);

    for (auto& b : mBoids)
        b.DrawSelf(game);

    return !(game->GetKey(olc::ESCAPE).bPressed);
}

bool Boids::Destroy()
{
    return true;
}