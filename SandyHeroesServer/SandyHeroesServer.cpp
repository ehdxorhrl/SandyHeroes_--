#include "stdafx.h"
#include "GameFramework.h"

GameFramework game_framework;

int main()
{
	std::wcout.imbue(std::locale("korean"));

	game_framework.Initialize();

	while (true)
	{
		game_framework.FrameAdvance();
	}
}



