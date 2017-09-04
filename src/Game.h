#pragma once

#include <stdexcept>
#include <string>
#include <memory>
#include <map>

#include <SDL.h>

#include "Board.h"
#include "ColourPalette.h"
#include "SoundEffects.h"
#include "GameController.h"

class Configuration;

class Game {
public:

	static void throwSDLException(std::string failure) {
		throw std::runtime_error(failure + ::SDL_GetError());
	}

	static void verifySDLCall(int returned, std::string failure) {
		if (returned < 0) {
			throwSDLException(failure);
		}
	}

	Game(const Configuration& configuration);

	void runLoop();
	void initialise();

private:
	enum {
		DisplayScale = 2,
		DisplayWidth = Board::RasterHeight,
		DisplayHeight = Board::RasterWidth
	};

	const Configuration& m_configuration;
	mutable Board m_board;
	ColourPalette m_colours;

	SDL_Window* m_window;
	SDL_Renderer* m_renderer;

	SDL_Texture* m_bitmapTexture;
	Uint32 m_pixelType;
	SDL_PixelFormat* m_pixelFormat;

	std::vector<uint32_t> m_pixels;

	int m_fps;
	Uint32 m_startTicks;
	Uint32 m_frames;
	bool m_vsync;

	SoundEffects m_effects;

	std::map<int, std::shared_ptr<GameController>> m_gameControllers;
	std::map<SDL_JoystickID, int> m_mappedControllers;

	int drawFrame(int prior);

	void configureBackground() const;
	void createBitmapTexture();

	int getScreenWidth() const {
		return DisplayWidth * DisplayScale;
	}

	int getScreenHeight() const {
		return DisplayHeight * DisplayScale;
	}

	int whichPlayer() const;

	void Board_UfoSound(const EightBit::EventArgs& event);
	void Board_ShotSound(const EightBit::EventArgs& event);
	void Board_PlayerDieSound(const EightBit::EventArgs& event);
	void Board_InvaderDieSound(const EightBit::EventArgs& event);
	void Board_ExtendSound(const EightBit::EventArgs& event);

	void Board_Walk1Sound(const EightBit::EventArgs& event);
	void Board_Walk2Sound(const EightBit::EventArgs& event);
	void Board_Walk3Sound(const EightBit::EventArgs& event);
	void Board_Walk4Sound(const EightBit::EventArgs& event);
	void Board_UfoDieSound(const EightBit::EventArgs& event);

	void Board_EnableAmplifier(const EightBit::EventArgs& event);
	void Board_DisableAmplifier(const EightBit::EventArgs& event);

	void handleKeyDown(SDL_Keycode key);
	void handleKeyUp(SDL_Keycode key);

	void handleJoyButtonDown(SDL_JoyButtonEvent event);
	void handleJoyButtonUp(SDL_JoyButtonEvent event);

	int chooseControllerIndex(int who) const;
	std::shared_ptr<GameController> chooseController(int who) const;

	void handleJoyLeftPress(int who, int joystick);
	void handleJoyRightPress(int who, int joystick);
	void handleJoyFirePress(int who, int joystick);

	void handleJoyLeftRelease(int who, int joystick);
	void handleJoyRightRelease(int who, int joystick);
	void handleJoyFireRelease(int who, int joystick);

	static void dumpRendererInformation();
	static void dumpRendererInformation(::SDL_RendererInfo info);
};
