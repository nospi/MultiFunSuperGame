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
	return true;
}

bool olcMFSG::OnUserDestroy()
{
	return true;
}