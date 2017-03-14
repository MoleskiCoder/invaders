#pragma once

#include <stdexcept>
#include <string>
#include <SDL.h>

#include "Board.h"
#include "ColourPalette.h"

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
		DisplayWidth = 256,
		DisplayHeight = 224
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

	bool m_finished;

	void update();
	void runFrame();
	bool finishedCycling(int cycles) const;
	void stop();

	void draw();
	void drawFrame();

	void configureBackground() const;
	void createBitmapTexture();

	int getScreenWidth() const {
		return DisplayWidth * DisplayScale;
	}

	int getScreenHeight() const {
		return DisplayHeight * DisplayScale;
	}

	void Board_PortWritten(const PortEventArgs& portEvent);
	void Cpu_ExecutingInstruction(const CpuEventArgs& cpuEvent);

	static void dumpRendererInformation();
	static void dumpRendererInformation(::SDL_RendererInfo info);
};
