#pragma once

#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "olcMFSG.h"

struct transform
{
    olc::vf2d position{ 0.0f, 0.0f };
    float rotation = 0.0f;
    float scale = 1.0f;

    transform() {}

    transform(const olc::vf2d& position, const float& angle = 0.0f, const float& scale = 1.0f)
        : position(position), rotation(angle), scale(scale) 
    { }

    void Rotate(float theta)
    {
        rotation += theta;
    }

    void Translate(const olc::vf2d& delta)
    {
        position += delta;
    }

    void Scale(const float& factor)
    {
        scale *= factor;
    }
};

#endif /* TRANSFORM_H */