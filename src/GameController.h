#pragma once

#include <array>
#include <map>
#include <SDL.h>

class GameController {
public:
	GameController(int index);
	virtual ~GameController();

	void startRumble();
	void stopRumble();

private:
	int m_index;
	SDL_GameController* m_gameController;

	void open();
	void close();

	SDL_Haptic* m_hapticController;
	bool m_hapticRumbleSupported;

	void openHapticController();
	void closeHapticController();
};