#pragma once
#ifndef CIRCLE_H
#define CIRCLE_H

#include "olcMFSG.h"

struct circle
{
    olc::vf2d center;
    float radius;

    circle() : center(0.0f, 0.0f), radius(1.0f) {}

    circle(const olc::vf2d& center, const float& radius) : center(center), radius(radius) {}

    bool contains(const olc::vf2d& point)
    {
        float r2 = radius * radius;
        float distance2 = (point.x - center.x) * (point.x - center.x) + (point.y - center.y) * (point.y - center.y);
        return distance2 <= r2;
    }

    bool collides(const circle& o)
    {
        float distance2 = (o.center - center).mag2();
        float radii2 = (radius * radius) + (o.radius * o.radius);
        return (distance2 <= radii2);
    }
};

#endif /* CIRCLE_H */