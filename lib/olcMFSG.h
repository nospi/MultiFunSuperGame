#pragma once
#include <olcPixelGameEngine.h>

class olcMFSG : public olc::PixelGameEngine
{
public:
	olcMFSG();
	~olcMFSG();
	bool OnUserCreate() override;
	bool OnUserUpdate(float) override;
	bool OnUserDestroy() override;
};

