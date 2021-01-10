#include "olcMFSG.h"
#include "scenes/mainmenu.h"
#include "scenes/noise.h"
#include "scenes/life.h"
#include "scenes/wormguy.h"
#include "scenes/matrix.h"
#include "scenes/boids.h"
#include "scenes/paddleball.h"

olcMFSG::olcMFSG()
{
	sAppName = "MultiFunSuperGame!";
}

olcMFSG::~olcMFSG()
{
	// nothin' to do!
}

bool olcMFSG::OnUserCreate()
{
	scenes.push_back(new MainMenu(0, this));
	scenes.push_back(new Noise(1, this));
	scenes.push_back(new Matrix(2, this));
	scenes.push_back(new Boids(3, this));
	scenes.push_back(new Life(4, this));
	scenes.push_back(new WormGuy(5, this));
	scenes.push_back(new PaddleBall(6, this));
	setActiveScene(scenes.front());
	return true;
}

bool olcMFSG::OnUserUpdate(float fElapsedTime)
{
	if (activeScene != nullptr)
		if (!activeScene->Update(fElapsedTime))
		{
			if (activeScene->id == 0)
				return false;
			activeScene->Destroy();
			setActiveScene(scenes.front());
		}
	return true;
}

bool olcMFSG::OnUserDestroy()
{
	for (auto& scene : scenes)
		delete scene;
	return true;
}

void olcMFSG::setActiveScene(scene* scn)
{
	scn->Create();
	activeScene = scn;
	sAppName = "MFSG: " + scn->title;
}

void olcMFSG::LoadScene(const int& id)
{
	auto it = std::find_if(scenes.begin(), scenes.end(), [id](scene* s){ return s->id == id; });
	if (it != scenes.end())
	{
		activeScene->Destroy();
		setActiveScene(*it);
	}
}

void olcMFSG::LoadScene(const std::string& title)
{
	auto it = std::find_if(scenes.begin(), scenes.end(), [title](scene* s){ return s->title == title; });
	if (it != scenes.end())
	{
		activeScene->Destroy();
		setActiveScene(*it);
	}
}

std::vector<scene*>* olcMFSG::GetScenes()
{
	return &scenes;
}

void olcMFSG::DrawShadowedString(const olc::vi2d& pos, const olc::vi2d& offset, const std::string& text, olc::Pixel txtColour, olc::Pixel shadowColour, uint32_t scale)
{
	DrawString(pos + offset, text, shadowColour, scale);
	DrawString(pos - (offset - olc::vi2d(1, 1)), text, txtColour, scale);
}