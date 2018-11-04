#include "stdafx.h"
#include "Game.h"

#include <memory>

int main(int, char*[]) {

	Configuration configuration;

#ifdef _DEBUG
	configuration.setDebugMode(true);
	configuration.setProfileMode(true);
	configuration.setDrawGraphics(false);
	configuration.setShowWatchdogOutput(true);
#endif

	try {
		auto game = std::make_unique<Game>(configuration);
		game->initialise();
		game->runLoop();
	} catch (const std::exception& error) {
		::SDL_LogError(::SDL_LOG_CATEGORY_APPLICATION, "%s", error.what());
		return 2;
	}

	return 0;
}