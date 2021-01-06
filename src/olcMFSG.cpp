#include "olcMFSG.h"

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
