#include "stdafx.h"

#include "Game.h"

int main(int argc, char* argv[]) {

	Configuration configuration;

#ifdef _DEBUG
	configuration.setDebugMode(true);
#endif

	Game game(configuration);
	game.initialise();

	game.runLoop();

	return 0;
}