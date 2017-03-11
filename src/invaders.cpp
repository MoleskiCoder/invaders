#include "stdafx.h"

#include "Game.h"

int main(int argc, char* argv[]) {

	Configuration configuration;

#ifdef _DEBUG
	configuration.setDebugMode(true);
#endif

	Game game(configuration);
	game.initialise();

	try {
		game.runLoop();
	} catch (std::exception& error) {
		::SDL_LogError(::SDL_LOG_CATEGORY_APPLICATION, "%s", error.what());
		return 2;
	}

	return 0;
}