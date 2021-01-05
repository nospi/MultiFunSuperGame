#define OLC_PGE_APPLICATION
#include "olcMFSG.h"

int main(int argc, char* argv[])
{
	int width = 1280;
	int height = 768;
	int xres = 1;
	int yres = 1;
	bool fullscreen = false;
	bool vsync = false;

	for (int i = 1; i < argc; i++)
	{
		std::string arg(argv[i]);
		if (arg == "-w")
			width = std::stoi(argv[i + 1]);
		else if (arg == "-h")
			height = std::stoi(argv[i + 1]);
		else if (arg == "-x")
			xres = std::stoi(argv[i + 1]);
		else if (arg == "-y")
			yres = std::stoi(argv[i + 1]);
		else if (arg == "-f")
			fullscreen = true;
		else if (arg == "-v")
			vsync = true;
	}

	olcMFSG game;
	game.Construct(width, height, xres, yres, fullscreen, vsync);
	game.Start();
	return 0;
}