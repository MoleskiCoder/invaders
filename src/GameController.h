#pragma once

#include <array>
#include <map>
#include <SDL.h>

class GameController final {
public:
	GameController(int index);
	virtual ~GameController();

	void startRumble();
	void stopRumble();

	static auto buildJoystickId(SDL_GameController* controller) {
		auto joystick = ::SDL_GameControllerGetJoystick(controller);
		return ::SDL_JoystickInstanceID(joystick);
	}

	auto getJoystickId() const {
		return buildJoystickId(m_gameController);
	}

private:
	int m_index;
	SDL_GameController* m_gameController = nullptr;

	void open();
	void close();

	SDL_Haptic* m_hapticController = nullptr;
	bool m_hapticRumbleSupported = false;

	void openHapticController();
	void closeHapticController();
};