#pragma once

#include "olcMFSG.h"
#include "scene.h"
#include "astar.h"

enum CellPathMask
{
    PATH_NORTH = 1,
    PATH_EAST  = 2,
    PATH_SOUTH = 4,
    PATH_WEST  = 8
};

class Mazes;

struct MazeCell
{
    olc::vi2d pos = { 0, 0 };
    bool visited = false;
    MazeCell* parent = nullptr;
    int neighbours = 0;

    MazeCell(int x, int y) : pos(x, y), visited(false) {}
    void Draw(olcMFSG*, Mazes*);
};

class Mazes : public scene
{
public:
    enum MazeMode
    {
        GENERATE = 0,
        ASTAR_SOLVE,
        FF_SOLVE,
        PLAYER_SOLVE,
        NUM_MODES
    };

public:
    Mazes(int, olcMFSG*);
    bool Create() override;
    bool Update(float) override;
    bool Destroy() override;
    int nCellWidth;
    int nCellHeight;

private:
    MazeMode mode = GENERATE;
    int nCols;
    int nRows;
    float fGenerationTime;
    float fSolveTime;

    std::vector<MazeCell> vMaze;
    auto getCell(int x, int y);
    std::vector<MazeCell*> getImmediateNeighbours(MazeCell* cell);
    void generate(const olc::vi2d starting_pos = { 0, 0 });
    
    astar::node* nodes = nullptr;
    astar::node* nodeStart = nullptr;
    astar::node* nodeEnd = nullptr;
    astar::node* getNode(int x, int y);
    std::vector<astar::node*> getNodeNeighbours(astar::node*);
    void solve_aStar();
    void DrawNode(astar::node*);
};