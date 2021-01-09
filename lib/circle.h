#pragma once
#ifndef CIRCLE_H
#define CIRCLE_H

#include "olcMFSG.h"

struct sCircle
{
    olc::vf2d center;
    float radius;

    sCircle(const olc::vf2d& center, const float& radius) : center(center), radius(radius) {}

    bool contains(const olc::vf2d& point)
    {
        float r2 = radius * radius;
        float distance2 = (point.x - center.x) * (point.x - center.x) + (point.y - center.y) * (point.y - center.y);
        return distance2 <= r2;
    }
};

#endif /* CIRCLE_H */