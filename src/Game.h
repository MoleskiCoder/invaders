#pragma once

#include <stdexcept>
#include <string>
#include <SDL.h>

#include "Board.h"
#include "ColourPalette.h"
#include "SoundEffects.h"

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
	Board m_board;
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

	void runRasterScan();
	void runVerticalBlank();
	
	void drawFrame();

	void runToLimit(int limit);
	bool finishedCycling(int limit, int cycles) const;
	void stop();

	void configureBackground() const;
	void createBitmapTexture();

	int getScreenWidth() const {
		return DisplayWidth * DisplayScale;
	}

	int getScreenHeight() const {
		return DisplayHeight * DisplayScale;
	}

	void Board_UfoSound(const EventArgs& event);
	void Board_ShotSound(const EventArgs& event);
	void Board_PlayerDieSound(const EventArgs& event);
	void Board_InvaderDieSound(const EventArgs& event);

	void Board_Walk1Sound(const EventArgs& event);
	void Board_Walk2Sound(const EventArgs& event);
	void Board_Walk3Sound(const EventArgs& event);
	void Board_Walk4Sound(const EventArgs& event);
	void Board_UfoDieSound(const EventArgs& event);

	void handleKeyDown(SDL_Keycode key);
	void handleKeyUp(SDL_Keycode key);

	static void dumpRendererInformation();
	static void dumpRendererInformation(::SDL_RendererInfo info);
};
