#include "scenes/mazes.h"
#include <stack>
#include <algorithm>
#include <chrono>

using astar::node;
using hrc = std::chrono::high_resolution_clock;


void MazeCell::Draw(olcMFSG* game, Mazes* scene)
{
    olc::vi2d cellSize = { scene->nCellWidth, scene->nCellHeight };
    olc::vi2d centerSize = cellSize / 4 * 3;
    olc::vi2d cornerSize = (cellSize - centerSize) / 2;
    olc::vi2d scaledPos = pos * cellSize + cellSize;
    
    // clear
    game->FillRect(scaledPos, cellSize, olc::VERY_DARK_GREY);

    // draw center
    game->FillRect(scaledPos + cornerSize, centerSize, olc::WHITE);

    // draw neighbour paths   
    if ((neighbours & PATH_NORTH) != 0)
        game->FillRect({ scaledPos.x + cornerSize.x, scaledPos.y }, { cellSize.x - (2 * cornerSize.x), cornerSize.y }, olc::WHITE);

    if ((neighbours & PATH_EAST) != 0)
        game->FillRect({ scaledPos.x + cornerSize.x + centerSize.x, scaledPos.y + cornerSize.y }, { cornerSize.x, cellSize.y - (2 * cornerSize.y) }, olc::WHITE);

    if ((neighbours & PATH_SOUTH) != 0)
        game->FillRect({ scaledPos.x + cornerSize.x, scaledPos.y + cornerSize.y + centerSize.y }, { cellSize.x - (2 * cornerSize.x), cornerSize.y }, olc::WHITE);

    if ((neighbours & PATH_WEST) != 0)
        game->FillRect({ scaledPos.x, scaledPos.y + cornerSize.y }, { cornerSize.x, cellSize.y - (2 * cornerSize.y) }, olc::WHITE);
}

Mazes::Mazes(int id, olcMFSG* game) : scene(id, "Mazes", game) {}

bool Mazes::Create()
{
    fGenerationTime = 0.0f;
    fSolveTime = 0.0f;

    nCellWidth = 32;
    nCellHeight = 32;
    nCols = game->ScreenWidth() / nCellWidth - 2;
    nRows = game->ScreenHeight() / nCellHeight - 2;

    for (int y = 0; y < nRows; y++)
        for (int x = 0; x < nCols; x++)
            vMaze.emplace_back(x, y);
    
    generate({ 0, 0 });

    return true;
}

bool Mazes::Destroy()
{
    if (nodes != nullptr)
        delete[] nodes;
    return true;
}

bool Mazes::Update(float fElapsedTime)
{
    if (game->GetKey(olc::K1).bPressed)
        mode = GENERATE;
    if (game->GetKey(olc::K2).bPressed)
        mode = ASTAR_SOLVE;

    switch (mode)
    {
    case GENERATE: 
        if (game->GetKey(olc::SPACE).bPressed)
            generate({ 0, 0 });
        break;

    case ASTAR_SOLVE:
        if (game->GetKey(olc::SPACE).bPressed)
        {
            generate({ 0, 0 });
        }
        break;

    case PLAYER_SOLVE: 
        break;

    case FF_SOLVE: 
    
        break;
    }

    game->Clear(0);

    for (auto& mc : vMaze)
        mc.Draw(game, this);
    
    if (mode == ASTAR_SOLVE)
    {
        if (nodeEnd != nullptr)
        {
            node* p = nodeEnd;
            while (p != nullptr)
            {
                DrawNode(p);
                p = p->parent;
            }
        }
    }

    std::string sGenTime = "Generation Time: " + std::to_string(fGenerationTime);
    std::string sSolveTime = "Solve Time: " + std::to_string(fSolveTime);
    
    game->DrawString({ 12, 12 }, sGenTime);
    game->DrawString({ game->ScreenWidth() - (int)sSolveTime.length() * 8 - 12, 12 }, sSolveTime);
    
    return !(game->GetKey(olc::ESCAPE).bPressed);
}

auto Mazes::getCell(int x, int y)
{
    return std::find_if(std::begin(vMaze), std::end(vMaze), [x, y](MazeCell mc){ return mc.pos.x == x && mc.pos.y == y; });
}

std::vector<MazeCell*> Mazes::getImmediateNeighbours(MazeCell* cell)
{
    std::vector<MazeCell*> neighbours;
    std::vector<olc::vi2d> possible = { { -1, 0 }, { 0, -1 }, { 1, 0 }, { 0, 1 } };
    for (auto p : possible)
    {
        p += cell->pos;
        auto mc = getCell(p.x, p.y);
        if (mc != std::end(vMaze))
            neighbours.push_back(&(*mc));
    }
    return neighbours;
};

void Mazes::generate(olc::vi2d starting_pos)
{
    int nVisited = 0;
    std::stack<MazeCell*> stk;

    auto t_g1 = hrc::now();

    // initialise maze in case it is already generated
    for (auto& mc : vMaze)
    {
        mc.neighbours = 0;
        mc.visited = false;
        mc.parent = nullptr;
    }

    // clamp starting position to a border cell (north/west borders only for now)
    // if ((starting_pos.x != 0 || starting_pos.x != nCols - 1) && (starting_pos.y != 0 || starting_pos.y != nRows - 1))
    if (starting_pos.x != 0 && starting_pos.y != 0)
        starting_pos = { -1, 0 };
    
    // get starting cell
    auto itr = std::find_if(vMaze.begin(), vMaze.end(), [starting_pos](MazeCell mc){ return mc.pos == starting_pos; });
    if (itr != std::end(vMaze))
    {
        itr->visited = true;
        nVisited++;
        stk.push(&(*itr));
    }

    // perform recursive backtrack algorithm
    while (nVisited < nRows * nCols)
    {
        // determine unvisited neighbours of current cell
        auto current = stk.top();
        auto neighbours = getImmediateNeighbours(current);
        std::vector<MazeCell*> unvisited;
        std::copy_if(neighbours.begin(), neighbours.end(), std::back_inserter(unvisited), [](MazeCell* n){ return !n->visited; });

        if (unvisited.size() == 0)
        {
            // backtrack
            stk.pop();
        }
        else
        {
            // randomly determine next cell
            MazeCell* next = unvisited[rand() % unvisited.size()];
            next->parent = current;
            next->visited = true;
            nVisited++;

            // set path masks
            olc::vi2d vDir = next->pos - current->pos;
            int direction = (int)(4 * (atan2f(vDir.y, vDir.x) / (2 * M_PI)) + 1);
            switch (direction)
            {
                case 0: current->neighbours |= PATH_NORTH; next->neighbours |= PATH_SOUTH; break;  // north
                case 1: current->neighbours |= PATH_EAST;  next->neighbours |= PATH_WEST;  break;  // east
                case 2: current->neighbours |= PATH_SOUTH; next->neighbours |= PATH_NORTH; break;  // south
                case 3: current->neighbours |= PATH_WEST;  next->neighbours |= PATH_EAST;  break;  // west
            }

            stk.push(next);
        }
    }

    auto t_g2 = hrc::now();
    auto t_s1 = hrc::now();

    // cleanup existing nodes if they exist
    if (nodes != nullptr)
        delete[] nodes;
    
    auto getNodePtr = [&](int x, int y){ return &nodes[y * nCols + x]; };

    // initialise list of nodes to match mazeCells
    nodes = new node[vMaze.size()];
    int i = 0;
    for (auto& mc : vMaze)
    {
        nodes[i].position = mc.pos;
        if ((mc.neighbours & PATH_NORTH) != 0)
            nodes[i].neighbours.push_back(getNodePtr(mc.pos.x, mc.pos.y - 1));
        if ((mc.neighbours & PATH_SOUTH) != 0)
            nodes[i].neighbours.push_back(getNodePtr(mc.pos.x, mc.pos.y + 1));
        if ((mc.neighbours & PATH_EAST) != 0)
            nodes[i].neighbours.push_back(getNodePtr(mc.pos.x + 1, mc.pos.y));
        if ((mc.neighbours & PATH_WEST) != 0)
            nodes[i].neighbours.push_back(getNodePtr(mc.pos.x - 1, mc.pos.y));
        i++;
    }

    nodeStart = getNode(0, 0);
    nodeEnd = getNode(nCols - 1, nRows - 1);

    solve_aStar();
    auto t_s2 = hrc::now();

    std::chrono::duration<double> gen_diff = t_g2 - t_g1;
    fGenerationTime = gen_diff.count();

    std::chrono::duration<double> solve_diff = t_s2 - t_s1;
    fSolveTime = solve_diff.count();
}

node* Mazes::getNode(int x, int y)
{ 
    return std::find_if(&nodes[0], &nodes[vMaze.size()], [x, y](node n){ 
        return n.position.x == x && n.position.y == y; 
    }); 
};

std::vector<node*> Mazes::getNodeNeighbours(node* n)
{
    std::vector<node*> neighbours;
    int nbMask = getCell(n->position.x, n->position.y)->neighbours;
    if ((nbMask & PATH_NORTH) != 0) neighbours.push_back(getNode(n->position.x - 0, n->position.y - 1));
    if ((nbMask & PATH_EAST) != 0)  neighbours.push_back(getNode(n->position.x + 1, n->position.y + 0));
    if ((nbMask & PATH_SOUTH) != 0) neighbours.push_back(getNode(n->position.x + 0, n->position.y + 1));
    if ((nbMask & PATH_WEST) != 0)  neighbours.push_back(getNode(n->position.x - 1, n->position.y - 0));
    return neighbours;
}

void Mazes::solve_aStar()
{
    // reset navigation state
    for (int i = 0; i < vMaze.size(); i++)
    {
        nodes[i].visited = false;
        nodes[i].globalGoal = INFINITY;
        nodes[i].localGoal = INFINITY;
        nodes[i].parent = nullptr;
    }

    auto distance = [](node* a, node* b)
    { 
        return (a->position - b->position).mag(); 
    };

    auto heuristic = [distance](node* a, node* b)
    { 
        return distance(a, b); 
    };    

    node* current = nodeStart;
    std::list<node*> lNotTestedNodes;

    current->localGoal = 0.0f;
    current->globalGoal = heuristic(current, nodeEnd);
    lNotTestedNodes.push_back(current);

    while (!lNotTestedNodes.empty())
    {
        // sort by global goal, ascending
        lNotTestedNodes.sort([](const node* lhs, const node* rhs){ return lhs->globalGoal < rhs->globalGoal; });
        
        // remove visited nodes
        while (!lNotTestedNodes.empty() && lNotTestedNodes.front()->visited)
            lNotTestedNodes.pop_front();
        
        // ensure some are left or abort
        if (lNotTestedNodes.empty())
            break;

        // front node is best chance to get to goal
        current = lNotTestedNodes.front();
        current->visited = true;

        // test neighbours
        auto neighbours = getNodeNeighbours(current);
        for (auto& nb : neighbours)
        {
            // only bother with unvisited, passable nodes
            if (!nb->visited && !nb->obstacle)
                lNotTestedNodes.push_back(nb);
            
            // determine whether current cell is a better parent for this neighbour
            float fPossiblyLowerGoal = current->localGoal + distance(current, nb);
            if (fPossiblyLowerGoal < nb->localGoal)
            {
                nb->parent = current;
                nb->localGoal = fPossiblyLowerGoal;
                nb->globalGoal = nb->localGoal + heuristic(nb, nodeEnd);
            }
        }
    }
}

void Mazes::DrawNode(node* n)
{
    olc::vi2d cellSize = { nCellWidth, nCellHeight };
    olc::vi2d centerSize = cellSize / 4 * 3;
    olc::vi2d cornerSize = (cellSize - centerSize) / 2;

    if (n->parent != nullptr)
        game->DrawLine(n->position * cellSize + cellSize + cellSize / 2, n->parent->position * cellSize + cellSize + cellSize / 2, olc::DARK_RED);

    if (n == nodeStart)
        game->FillCircle(n->position * cellSize + cellSize + cellSize / 2, centerSize.mag() / 4, olc::RED);
    else if (n->position == nodeEnd->position)
        game->FillCircle(n->position * cellSize + cellSize + cellSize / 2, centerSize.mag() / 4, olc::GREEN);
    else
        game->FillCircle(n->position * cellSize + cellSize + cellSize / 2, centerSize.mag() / 4, olc::DARK_BLUE);
}