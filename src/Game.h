#pragma once

#include <cstdint>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include <SDL.h>

#include <EventArgs.h>

#include "Board.h"
#include "ColourPalette.h"
#include "SoundEffects.h"
#include "GameController.h"

class Configuration;

class Game final {
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
	~Game();

	void runLoop();
	void initialise();

private:
	enum {
		DisplayScale = 2,
		DisplayWidth = Board::RasterHeight,
		DisplayHeight = Board::RasterWidth
	};

	const Configuration& m_configuration;
	Board m_board;
	ColourPalette m_colours;

	std::shared_ptr<SDL_Window> m_window;
	std::shared_ptr<SDL_Renderer> m_renderer;
	std::shared_ptr<SDL_PixelFormat> m_pixelFormat;
	std::shared_ptr<SDL_Texture> m_bitmapTexture;
	Uint32 m_pixelType = SDL_PIXELFORMAT_ARGB8888;

	std::vector<uint32_t> m_pixels;

	int m_fps;
	Uint32 m_startTicks = 0;
	Uint32 m_frames = 0;
	bool m_vsync = false;

	SoundEffects m_effects;

	std::map<int, std::shared_ptr<GameController>> m_gameControllers;
	std::map<SDL_JoystickID, int> m_mappedControllers;

	int drawFrame(int prior);

	void configureBackground() const;
	void createBitmapTexture();

	static constexpr auto getScreenWidth() {
		return DisplayWidth * DisplayScale;
	}

	static constexpr auto getScreenHeight() {
		return DisplayHeight * DisplayScale;
	}

	int whichPlayer();

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

	static void dumpRendererInformation();
	static void dumpRendererInformation(::SDL_RendererInfo info);
};
