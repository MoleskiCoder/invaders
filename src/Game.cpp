#include "stdafx.h"
#include "Game.h"

Game::Game(const Configuration& configuration)
:	m_configuration(configuration),
	m_board(configuration) {
}

void Game::initialise() {

	verifySDLCall(::SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC), "Failed to initialise SDL: ");

	m_board.initialise();

	auto windowWidth = getScreenWidth();
	auto windowHeight = getScreenHeight();

	m_window = ::SDL_CreateWindow(
		"Space Invaders",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		windowWidth, windowHeight,
		SDL_WINDOW_SHOWN);

	if (m_window == nullptr) {
		throwSDLException("Unable to create window: ");
	}

	::SDL_DisplayMode mode;
	verifySDLCall(::SDL_GetWindowDisplayMode(m_window, &mode), "Unable to obtain window information");

	m_vsync = m_configuration.getVsyncLocked();
	Uint32 rendererFlags = 0;
	if (m_vsync) {
		auto required = m_configuration.getFramesPerSecond();
		if (required == mode.refresh_rate) {
			rendererFlags |= SDL_RENDERER_PRESENTVSYNC;
			::SDL_Log("Attempting to use SDL_RENDERER_PRESENTVSYNC");
		}
		else {
			m_vsync = false;
			::SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Display refresh rate is incompatible with required rate (%d)", required);
		}
	}
	m_renderer = ::SDL_CreateRenderer(m_window, -1, rendererFlags);
	if (m_renderer == nullptr) {
		throwSDLException("Unable to create renderer: ");
	}

	::SDL_Log("Available renderers:");
	dumpRendererInformation();

	::SDL_RendererInfo info;
	verifySDLCall(::SDL_GetRendererInfo(m_renderer, &info), "Unable to obtain renderer information");
	::SDL_Log("Using renderer:");
	dumpRendererInformation(info);

	if (m_vsync) {
		if ((info.flags & SDL_RENDERER_PRESENTVSYNC) == 0) {
			::SDL_LogWarn(::SDL_LOG_CATEGORY_APPLICATION, "Renderer does not support VSYNC, reverting to timed delay loop.");
			m_vsync = false;
		}
	}

	m_pixelFormat = ::SDL_AllocFormat(m_pixelType);
	if (m_pixelFormat == nullptr) {
		throwSDLException("Unable to allocate pixel format: ");
	}
	m_colours.load(m_pixelFormat);

	configureBackground();
	createBitmapTexture();
}

void Game::configureBackground() const {
	Uint8 r, g, b;
	::SDL_GetRGB(m_colours.getColour(0), m_pixelFormat, &r, &g, &b);
	verifySDLCall(::SDL_SetRenderDrawColor(m_renderer, r, g, b, SDL_ALPHA_OPAQUE), "Unable to set render draw colour");
}

void Game::createBitmapTexture() {
	m_bitmapTexture = ::SDL_CreateTexture(m_renderer, m_pixelType, SDL_TEXTUREACCESS_STREAMING, DisplayWidth, DisplayHeight);
	if (m_bitmapTexture == nullptr) {
		throwSDLException("Unable to create bitmap texture");
	}
	m_pixels.resize(DisplayWidth * DisplayHeight);
}

void Game::runLoop() {

	m_frames = 0UL;
	m_startTicks = ::SDL_GetTicks();

	auto& cpu = m_board.getCPUMutable();

	while (!cpu.isHalted()) {
		::SDL_Event e;
		while (::SDL_PollEvent(&e)) {
			switch (e.type) {
			case SDL_QUIT:
				cpu.halt();
				break;
			case SDL_KEYDOWN:
				//handleKeyDown(e.key.keysym.sym);
				break;
			case SDL_KEYUP:
				//handleKeyUp(e.key.keysym.sym);
				break;
			case SDL_JOYDEVICEADDED:
				SDL_Log("Joystick device added");
				//m_gameController.open();
				break;
			case SDL_JOYDEVICEREMOVED:
				SDL_Log("Joystick device removed");
				//m_gameController.close();
				break;
			}
		}

		runRasterScan();

		m_board.getCPUMutable().interrupt(1);	// beginning of the vertical blank

		drawFrame();

		if (m_vsync) {
			::SDL_RenderPresent(m_renderer);
		} else {
			const auto elapsedTicks = ::SDL_GetTicks() - m_startTicks;
			const auto neededTicks = (++m_frames / (float)m_fps) * 1000.0;
			auto sleepNeeded = (int)(neededTicks - elapsedTicks);
			if (sleepNeeded > 0) {
				::SDL_Delay(sleepNeeded);
			}
		}

		runVerticalBlank();

		m_board.getCPUMutable().interrupt(2);	// end of the vertical blank
	}
}

void Game::runRasterScan() {
	runToLimit(m_configuration.getCyclesDuringRasterScan());
}

void Game::runVerticalBlank() {
	runToLimit(m_configuration.getCyclesDuringVerticalBlank());
}

void Game::runToLimit(int limit) {
	for (int cycles = 0; !finishedCycling(limit, cycles); ++cycles) {
		m_board.getCPUMutable().step();
	}
}

bool Game::finishedCycling(int limit, int cycles) const {
	auto exhausted = cycles > limit;
	return exhausted || m_board.getCPU().isHalted();
}

void Game::stop() {
	m_board.getCPUMutable().halt();
}

void Game::drawFrame() {

	auto memory = m_board.getMemory();
	int address = Memory::VideoRam;

	auto pixelIndex = 0;

	auto bytesPerScanLine = DisplayWidth / 8;
	for (int y = 0; y < DisplayHeight; ++y) {
		for (int byte = 0; byte < bytesPerScanLine; ++byte) {
			std::bitset<8> source = memory.get(++address);
			for (int bit = 0; bit < 8; ++bit) {
				const bool& pixel = source[bit];
				m_pixels[pixelIndex++] = m_colours.getColour(pixel);
			}
		}
	}
	
	verifySDLCall(::SDL_UpdateTexture(m_bitmapTexture, NULL, &m_pixels[0], DisplayWidth * sizeof(Uint32)), "Unable to update texture: ");
	verifySDLCall(::SDL_RenderCopy(m_renderer, m_bitmapTexture, NULL, NULL), "Unable to copy texture to renderer");
}

void Game::dumpRendererInformation() {
	auto count = ::SDL_GetNumRenderDrivers();
	for (int i = 0; i < count; ++i) {
		::SDL_RendererInfo info;
		verifySDLCall(::SDL_GetRenderDriverInfo(i, &info), "Unable to obtain renderer information");
		dumpRendererInformation(info);
	}
}

void Game::dumpRendererInformation(::SDL_RendererInfo info) {
	auto name = info.name;
	auto flags = info.flags;
	int software = (flags & SDL_RENDERER_SOFTWARE) != 0;
	int accelerated = (flags & SDL_RENDERER_ACCELERATED) != 0;
	int vsync = (flags & SDL_RENDERER_PRESENTVSYNC) != 0;
	int targetTexture = (flags & SDL_RENDERER_TARGETTEXTURE) != 0;
	::SDL_Log("%s: software=%d, accelerated=%d, vsync=%d, target texture=%d", name, software, accelerated, vsync, targetTexture);
}
