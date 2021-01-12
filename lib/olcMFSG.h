#pragma once
#include <olcPixelGameEngine.h>
#include <vector>

#define M_PI 3.14159265358979323846264338327950288

inline float deg2rad(float deg) { return deg * 180.0f / M_PI; }

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
	
	void WrapCoordinates(olc::vf2d& coords);
	void WrapCoordinates(int32_t x, int32_t y, int32_t& ox, int32_t& oy);
	bool Draw(int32_t x, int32_t y, olc::Pixel p = olc::WHITE) override;
	bool bScreenWrapEnabled;

private:
	std::vector<scene*> scenes;
	scene* activeScene = nullptr;
	void setActiveScene(scene*);
};