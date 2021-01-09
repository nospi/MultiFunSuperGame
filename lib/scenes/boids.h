#pragma once

#include "scene.h"
#include "transform.h"
#include "circle.h"
#include <vector>

struct sBoid;

class Boids : public scene
{
public:
    static int MaxBoidCount;
    static float MaxSpeed;
    static float ForceStrength_Mouse;
    static float ForceStrength_Alignment;
    static float ForceStrength_Cohesion;
    static float ForceStrength_Separation;
    static float Radius_Separation;
    static float Radius_Cohesion;
    static float Radius_Alignment;

    Boids(int id, olcMFSG* game);
    bool Create() override;
    bool Update(float) override;
    bool Destroy() override;

private:
    std::vector<sBoid> mBoids;
};
