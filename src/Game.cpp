#include "stdafx.h"
#include "Game.h"
#include "Disassembler.h"

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

	m_board.getIO().WrittenPort.connect(std::bind(&Game::Board_PortWritten, this, std::placeholders::_1));
	m_board.getCPU().ExecutingInstruction.connect(std::bind(&Game::Cpu_ExecutingInstruction, this, std::placeholders::_1));
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

	while (!m_finished) {
		::SDL_Event e;
		while (::SDL_PollEvent(&e)) {
			switch (e.type) {
			case SDL_QUIT:
				m_finished = true;
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

		update();

		if (!m_vsync) {
			const auto elapsedTicks = ::SDL_GetTicks() - m_startTicks;
			const auto neededTicks = (++m_frames / (float)m_fps) * 1000.0;
			auto sleepNeeded = (int)(neededTicks - elapsedTicks);
			if (sleepNeeded > 0) {
				::SDL_Delay(sleepNeeded);
			}
		}
	}
}

void Game::update() {
	runFrame();
	draw();
}

void Game::runFrame() {
	for (int cycles = 0; !finishedCycling(cycles); ++cycles) {
		m_board.getCPU().step();
	}
}

bool Game::finishedCycling(int cycles) const {
	auto limit = m_configuration.getCyclesPerFrame();
	auto exhausted = cycles > limit;
	return exhausted || m_finished;
}

void Game::stop() {
	m_finished = true;
}

void Game::draw() {
	drawFrame();
	if (m_vsync) {
		::SDL_RenderPresent(m_renderer);
	}
}

void Game::drawFrame() {

	for (int y = 0; y < DisplayHeight; y++) {
		auto rowOffset = y * DisplayWidth;
		for (int x = 0; x < DisplayWidth; x++) {
			auto pixelIndex = x + rowOffset;
			int colourIndex = 1;
			m_pixels[pixelIndex] = m_colours.getColour(colourIndex);
		}
	}

	verifySDLCall(::SDL_UpdateTexture(m_bitmapTexture, NULL, &m_pixels[0], DisplayWidth * sizeof(Uint32)), "Unable to update texture: ");
	verifySDLCall(::SDL_RenderCopy(m_renderer, m_bitmapTexture, NULL, NULL), "Unable to copy texture to renderer");
}

void Game::Board_PortWritten(const PortEventArgs& portEvent) {
	auto port = portEvent.getPort();
	auto value = m_board.getIO().readOutputPort(port);
	std::cout << "Port written: Port: " << (int)port << ", value: " << (int)value << std::endl;
}

void Game::Cpu_ExecutingInstruction(const CpuEventArgs& cpuEvent) {
	std::cout
		<< Disassembler::state(cpuEvent.getCpu())
		<< "\t"
		<< Disassembler::disassemble(cpuEvent.getCpu())
		<< std::endl;
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
