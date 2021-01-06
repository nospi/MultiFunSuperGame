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
	void DrawShadowedString(const olc::vi2d& pos, const olc::vi2d& offset, const std::string& text, olc::Pixel txtColour = olc::WHITE, olc::Pixel shadowColour = olc::BLACK, uint32_t scale = 1U);

private:
	std::vector<scene*> scenes;
	scene* activeScene = nullptr;
	void setActiveScene(scene*);
};