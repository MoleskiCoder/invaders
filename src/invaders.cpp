#include "stdafx.h"

#include "Game.h"

int main(int argc, char* argv[]) {

	Configuration configuration;

	Game game(configuration);
	game.initialise();

	game.runLoop();

	return 0;
}