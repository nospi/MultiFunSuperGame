#pragma once
#ifndef RECT_H
#define RECT_H

#include "olcMFSG.h"

template<class T = olc::vi2d>
struct rect
{
    T position{ 0, 0 };
    T size{ 1, 1 };

    rect() : position({ 0, 0 }), size({ 1, 1 }) {}
    rect(const T& pos, const T& sz) : position(pos), size(sz) {}

    T topLeft() const
    {
        return position;
    }

    T bottomRight() const
    {
        return position + size;
    }

    bool contains(const T& point)
    {
        return position.x <= point.x && point.x <= position.x + size.x &&
            position.y <= point.y && point.y <= position.y + size.y;
    }

    bool collides(const rect<T>& o)
    {
        if (topLeft().x < o.bottomRight().x && bottomRight().x > o.topLeft().x &&
            topLeft().y < o.bottomRight().y && bottomRight().y > o.topLeft().y)
        {
            return true;
        }
        return false;
    }
};

#endif /* RECT_H */