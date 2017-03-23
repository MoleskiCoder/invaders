#include "stdafx.h"
#include "Game.h"

Game::Game(const Configuration& configuration)
:	m_configuration(configuration),
	m_board(configuration),
	m_window(nullptr),
	m_renderer(nullptr),
	m_bitmapTexture(nullptr),
	m_pixelType(SDL_PIXELFORMAT_ARGB8888),
	m_pixelFormat(nullptr),
	m_fps(configuration.getFramesPerSecond()),
	m_startTicks(0),
	m_frames(0),
	m_vsync(false),
	m_effects(configuration) {
}

void Game::initialise() {

	verifySDLCall(::SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC), "Failed to initialise SDL: ");

	m_board.initialise();

	m_board.UfoSound.connect(std::bind(&Game::Board_UfoSound, this, std::placeholders::_1));
	m_board.ShotSound.connect(std::bind(&Game::Board_ShotSound, this, std::placeholders::_1));
	m_board.PlayerDieSound.connect(std::bind(&Game::Board_PlayerDieSound, this, std::placeholders::_1));
	m_board.InvaderDieSound.connect(std::bind(&Game::Board_InvaderDieSound, this, std::placeholders::_1));

	m_board.Walk1Sound.connect(std::bind(&Game::Board_Walk1Sound, this, std::placeholders::_1));
	m_board.Walk2Sound.connect(std::bind(&Game::Board_Walk2Sound, this, std::placeholders::_1));
	m_board.Walk3Sound.connect(std::bind(&Game::Board_Walk3Sound, this, std::placeholders::_1));
	m_board.Walk4Sound.connect(std::bind(&Game::Board_Walk4Sound, this, std::placeholders::_1));
	m_board.UfoDieSound.connect(std::bind(&Game::Board_UfoDieSound, this, std::placeholders::_1));

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
		} else {
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
				handleKeyDown(e.key.keysym.sym);
				break;
			case SDL_KEYUP:
				handleKeyUp(e.key.keysym.sym);
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

		if (m_configuration.getMachineMode() == Configuration::SpaceInvaders)
			m_board.getCPUMutable().interrupt(1);	// beginning of the vertical blank

		if (m_configuration.isDrawGraphics())
			drawFrame();

		::SDL_RenderPresent(m_renderer);
		if (!m_vsync) {
			const auto elapsedTicks = ::SDL_GetTicks() - m_startTicks;
			const auto neededTicks = (++m_frames / (float)m_fps) * 1000.0;
			auto sleepNeeded = (int)(neededTicks - elapsedTicks);
			if (sleepNeeded > 0) {
				::SDL_Delay(sleepNeeded);
			}
		}

		runVerticalBlank();

		if (m_configuration.getMachineMode() == Configuration::SpaceInvaders)
			m_board.getCPUMutable().interrupt(2);	// end of the vertical blank
	}
}

void Game::handleKeyDown(SDL_Keycode key) {
	switch (key) {
	case SDLK_1:
		m_board.press1P();
		break;
	case SDLK_2:
		m_board.press2P();
		break;
	case SDLK_3:
		m_board.pressCredit();
		break;
	case SDLK_z:
		m_board.pressLeft1P();
		break;
	case SDLK_x:
		m_board.pressRight1P();
		break;
	case SDLK_BACKSLASH:
		m_board.pressShoot1P();
		break;
	case SDLK_COMMA:
		m_board.pressLeft2P();
		break;
	case SDLK_PERIOD:
		m_board.pressRight2P();
		break;
	case SDLK_SLASH:
		m_board.pressShoot2P();
		break;
	}
}

void Game::handleKeyUp(SDL_Keycode key) {
	switch (key) {
	case SDLK_1:
		m_board.release1P();
		break;
	case SDLK_2:
		m_board.release2P();
		break;
	case SDLK_3:
		m_board.releaseCredit();
		break;
	case SDLK_z:
		m_board.releaseLeft1P();
		break;
	case SDLK_x:
		m_board.releaseRight1P();
		break;
	case SDLK_BACKSLASH:
		m_board.releaseShoot1P();
		break;
	case SDLK_COMMA:
		m_board.releaseLeft2P();
		break;
	case SDLK_PERIOD:
		m_board.releaseRight2P();
		break;
	case SDLK_SLASH:
		m_board.releaseShoot2P();
		break;
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

	auto black = m_colours.getColour(ColourPalette::Black);

	// This code handles the display rotation
	auto bytesPerScanLine = Board::RasterWidth / 8;
	for (int inputY = 0; inputY < Board::RasterHeight; ++inputY) {
		for (int byte = 0; byte < bytesPerScanLine; ++byte) {
			auto video = memory.get(++address);
			for (int bit = 0; bit < 8; ++bit) {
				auto inputX = byte * 8 + bit;
				auto outputX = inputY;
				auto outputY = (Board::RasterWidth - inputX - 1);
				auto outputPixel = outputX + outputY * DisplayWidth;
				auto mask = 1 << bit;
				auto inputPixel = video & mask;
				auto colour = inputPixel ? m_colours.getColour(ColourPalette::calculateColour(outputX, outputY)) : black;
				m_pixels[outputPixel] = colour;
			}
		}
	}
	
	verifySDLCall(::SDL_UpdateTexture(m_bitmapTexture, NULL, &m_pixels[0], DisplayWidth * sizeof(Uint32)), "Unable to update texture: ");

	verifySDLCall(
		::SDL_RenderCopy(m_renderer, m_bitmapTexture, nullptr, nullptr), 
		"Unable to copy texture to renderer");
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

void Game::Board_UfoSound(const EventArgs& event) {
	m_effects.playUfo();
}

void Game::Board_ShotSound(const EventArgs& event) {
	m_effects.playShot();
}

void Game::Board_PlayerDieSound(const EventArgs& event) {
	m_effects.playPlayerDie();
}

void Game::Board_InvaderDieSound(const EventArgs& event) {
	m_effects.playInvaderDie();
}

void Game::Board_Walk1Sound(const EventArgs& event) {
	m_effects.playWalk1();
}

void Game::Board_Walk2Sound(const EventArgs& event) {
	m_effects.playWalk2();
}

void Game::Board_Walk3Sound(const EventArgs& event) {
	m_effects.playWalk3();
}

void Game::Board_Walk4Sound(const EventArgs& event) {
	m_effects.playWalk4();
}

void Game::Board_UfoDieSound(const EventArgs& event) {
	m_effects.playUfoDie();
}
