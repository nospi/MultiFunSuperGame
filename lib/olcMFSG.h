#pragma once
#include <olcPixelGameEngine.h>
#include <vector>

struct scene;

class olcMFSG : public olc::PixelGameEngine
{
public:
	olcMFSG();
	~olcMFSG();
	bool OnUserCreate() override;
	bool OnUserUpdate(float) override;
	bool OnUserDestroy() override;
	void LoadScene(const int& id);
	void LoadScene(const std::string& title);
	std::vector<scene*>* GetScenes();
private:
	std::vector<scene*> scenes;
	scene* activeScene = nullptr;
	void setActiveScene(scene*);
};