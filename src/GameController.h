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

	SDL_JoystickID getJoystickId() const {
		return buildJoystickId(m_gameController);
	}

	static SDL_JoystickID buildJoystickId(SDL_GameController* controller) {
		auto joystick = ::SDL_GameControllerGetJoystick(controller);
		return ::SDL_JoystickInstanceID(joystick);
	}

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