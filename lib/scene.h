#pragma once
#ifndef SCENE_H
#define SCENE_H

#include <string>
#include "olcMFSG.h"

struct scene
{
    int id = -1;
    std::string title = "New Scene";
    olcMFSG* game = nullptr;

    scene(int id, const std::string& title, olcMFSG* game) 
        : id(id), title(title), game(game) {}
    
    virtual bool Create() = 0;
    virtual bool Update(float) = 0;
    virtual bool Destroy() { return true; };
};

#endif /* ifndef SCENE_H */