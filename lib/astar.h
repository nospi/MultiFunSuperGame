#include "olcMFSG.h"
#include <list>
// #include <corecrt_math.h>

namespace astar
{

    struct node
    {
        olc::vf2d position;
        bool visited = false;
        bool obstacle = false;
        float localGoal = INFINITY;
        float globalGoal = INFINITY;
        std::vector<node*> neighbours;
        node* parent = nullptr;

        node() : position(0, 0) {}
        node(int x, int y) : position((float)x, (float)y) {}
        node(float x, float y) : position(x, y) {}
    };

};