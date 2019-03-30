#include "stdafx.h"
#include "Game.h"

Game::Game(const Configuration& configuration)
:	m_configuration(configuration),
	m_board(configuration),
	m_fps(configuration.getFramesPerSecond()),
	m_effects(configuration) {
	verifySDLCall(::SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC), "Failed to initialise SDL: ");
}

Game::~Game() {
    ::SDL_Quit();
}

void Game::initialise() {

	m_board.initialise();

	m_board.UfoSound.connect(std::bind(&Game::Board_UfoSound, this, std::placeholders::_1));
	m_board.ShotSound.connect(std::bind(&Game::Board_ShotSound, this, std::placeholders::_1));
	m_board.PlayerDieSound.connect(std::bind(&Game::Board_PlayerDieSound, this, std::placeholders::_1));
	m_board.InvaderDieSound.connect(std::bind(&Game::Board_InvaderDieSound, this, std::placeholders::_1));
	m_board.ExtendSound.connect(std::bind(&Game::Board_ExtendSound, this, std::placeholders::_1));

	m_board.Walk1Sound.connect(std::bind(&Game::Board_Walk1Sound, this, std::placeholders::_1));
	m_board.Walk2Sound.connect(std::bind(&Game::Board_Walk2Sound, this, std::placeholders::_1));
	m_board.Walk3Sound.connect(std::bind(&Game::Board_Walk3Sound, this, std::placeholders::_1));
	m_board.Walk4Sound.connect(std::bind(&Game::Board_Walk4Sound, this, std::placeholders::_1));
	m_board.UfoDieSound.connect(std::bind(&Game::Board_UfoDieSound, this, std::placeholders::_1));

	m_board.EnableAmplifier.connect(std::bind(&Game::Board_EnableAmplifier, this, std::placeholders::_1));
	m_board.DisableAmplifier.connect(std::bind(&Game::Board_DisableAmplifier, this, std::placeholders::_1));

	m_window.reset(::SDL_CreateWindow(
		"Space Invaders",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		DisplayHeight * DisplayScale, DisplayWidth * DisplayScale,
		SDL_WINDOW_SHOWN), ::SDL_DestroyWindow);

	if (m_window == nullptr) {
		throwSDLException("Unable to create window: ");
	}

	::SDL_DisplayMode mode;
	verifySDLCall(::SDL_GetWindowDisplayMode(m_window.get(), &mode), "Unable to obtain window information");

	m_vsync = m_configuration.getVsyncLocked();
	Uint32 rendererFlags = 0;
	if (m_vsync) {
		const auto required = m_configuration.getFramesPerSecond();
		if (required == mode.refresh_rate) {
			rendererFlags |= SDL_RENDERER_PRESENTVSYNC;
			::SDL_Log("Attempting to use SDL_RENDERER_PRESENTVSYNC");
		} else {
			m_vsync = false;
			::SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Display refresh rate is incompatible with required rate (%d)", required);
		}
	}
	m_renderer.reset(::SDL_CreateRenderer(m_window.get(), -1, rendererFlags), ::SDL_DestroyRenderer);
	if (m_renderer == nullptr) {
		throwSDLException("Unable to create renderer: ");
	}

	::SDL_RendererInfo info;
	verifySDLCall(::SDL_GetRendererInfo(m_renderer.get(), &info), "Unable to obtain renderer information");

	if (m_vsync) {
		if ((info.flags & SDL_RENDERER_PRESENTVSYNC) == 0) {
			::SDL_LogWarn(::SDL_LOG_CATEGORY_APPLICATION, "Renderer does not support VSYNC, reverting to timed delay loop.");
			m_vsync = false;
		}
	}

	m_pixelFormat.reset(::SDL_AllocFormat(m_pixelType), ::SDL_FreeFormat);
	if (m_pixelFormat == nullptr) {
		throwSDLException("Unable to allocate pixel format: ");
	}
	m_colours.load(m_pixelFormat.get());

	createGelPixels();
	configureBackground();
	createBitmapTexture();
}

void Game::createGelPixels() {
	for (int y = 0; y < DisplayHeight; ++y) {
		for (int x = 0; x < DisplayWidth; ++x) {
			const auto colourIndex = ColourPalette::calculateColour(y, Board::RasterWidth - x - 1);
			m_gel[x + (y * DisplayWidth)] = m_colours.getColour(colourIndex);
		}
	}
}

void Game::configureBackground() const {
	Uint8 r, g, b;
	::SDL_GetRGB(m_colours.getColour(0), m_pixelFormat.get(), &r, &g, &b);
	verifySDLCall(::SDL_SetRenderDrawColor(m_renderer.get(), r, g, b, SDL_ALPHA_OPAQUE), "Unable to set render draw colour");
}

void Game::createBitmapTexture() {
	m_bitmapTexture.reset(::SDL_CreateTexture(m_renderer.get(), m_pixelType, SDL_TEXTUREACCESS_STREAMING, Board::RasterWidth, Board::RasterHeight), ::SDL_DestroyTexture);
	if (m_bitmapTexture == nullptr) {
		throwSDLException("Unable to create bitmap texture");
	}
}

void Game::runLoop() {

	m_frames = 0UL;
	m_startTicks = ::SDL_GetTicks();

	m_board.raisePOWER();

	auto cycles = 0;
	auto& cpu = m_board.CPU();
	while (cpu.powered()) {
		::SDL_Event e;
		while (::SDL_PollEvent(&e)) {
			switch (e.type) {
			case SDL_QUIT:
				m_board.lowerPOWER();
				break;
			case SDL_KEYDOWN:
				handleKeyDown(e.key.keysym.sym);
				break;
			case SDL_KEYUP:
				handleKeyUp(e.key.keysym.sym);
				break;
			case SDL_JOYBUTTONDOWN:
				handleJoyButtonDown(e.jbutton);
				break;
			case SDL_JOYBUTTONUP:
				handleJoyButtonUp(e.jbutton);
				break;
			case SDL_JOYDEVICEADDED: {
					const auto which = e.jdevice.which;
					SDL_assert(m_gameControllers.find(which) == m_gameControllers.end());
					auto controller = std::make_shared<GameController>(which);
					const auto joystickId = controller->getJoystickId();
					m_gameControllers[which] = controller;
					SDL_assert(m_mappedControllers.find(joystickId) == m_mappedControllers.end());
					m_mappedControllers[joystickId] = which;
					SDL_Log("Joystick device %d added (%zd controllers)", which, m_gameControllers.size());
				}
				break;
			case SDL_JOYDEVICEREMOVED: {
					const auto which = e.jdevice.which;
					const auto found = m_gameControllers.find(which);
					SDL_assert(found != m_gameControllers.end());
					auto controller = found->second;
					const auto joystickId = controller->getJoystickId();
					m_mappedControllers.erase(joystickId);
					m_gameControllers.erase(which);
					SDL_Log("Joystick device %d removed (%zd controllers)", which, m_gameControllers.size());
				}
				break;
			}
		}

		cycles = drawFrame(cycles);
		displayTexture();

		++m_frames;

		if (!m_vsync) {
			const auto elapsedTicks = ::SDL_GetTicks() - m_startTicks;
			const auto neededTicks = (m_frames / (float)m_fps) * 1000.0;
			const auto sleepNeeded = (int)(neededTicks - elapsedTicks);
			if (sleepNeeded > 0) {
				::SDL_Delay(sleepNeeded);
			}
		}
	}
}

// -1 if no controllers, otherwise index
int Game::chooseControllerIndex(const int who) const {
	const auto count = m_gameControllers.size();
	if (count == 0)
		return -1;
	auto firstController = m_gameControllers.cbegin();
	if (count == 1 || (who == 1))
		return firstController->first;
	auto secondController = (++firstController)->first;
	return secondController;
}

std::shared_ptr<GameController> Game::chooseController(const int who) const {
	const auto which = chooseControllerIndex(who);
	if (which == -1)
		return nullptr;
	const auto found = m_gameControllers.find(which);
	SDL_assert(found != m_gameControllers.cend());
	return found->second;
}

void Game::handleJoyButtonDown(const SDL_JoyButtonEvent event) {
	const auto joystickId = event.which;
	auto controllerIndex = m_mappedControllers[joystickId];
	const auto value = event.button;
	const auto who = whichPlayer();
	if (chooseControllerIndex(who) == controllerIndex) {
		switch (value) {
		case SDL_CONTROLLER_BUTTON_A:
			who == 1 ? m_board.pressShoot1P() : m_board.pressShoot2P();
			break;
		case SDL_CONTROLLER_BUTTON_BACK:
			who == 1 ? m_board.pressLeft1P() : m_board.pressLeft2P();
			break;
		case SDL_CONTROLLER_BUTTON_GUIDE:
			who == 1 ? m_board.pressRight1P() : m_board.pressRight2P();
			break;
		}
	}
}

void Game::handleJoyButtonUp(const SDL_JoyButtonEvent event) {
	const auto joystickId = event.which;
	auto controllerIndex = m_mappedControllers[joystickId];
	const auto value = event.button;
	const auto who = whichPlayer();
	if (chooseControllerIndex(who) == controllerIndex) {
		switch (value) {
		case SDL_CONTROLLER_BUTTON_A:
			who == 1 ? m_board.releaseShoot1P() : m_board.releaseShoot2P();
			break;
		case SDL_CONTROLLER_BUTTON_BACK:
			who == 1 ? m_board.releaseLeft1P() : m_board.releaseLeft2P();
			break;
		case SDL_CONTROLLER_BUTTON_GUIDE:
			who == 1 ? m_board.releaseRight1P() : m_board.releaseRight2P();
			break;
		}
	}
}

void Game::handleKeyDown(const SDL_Keycode key) noexcept {
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

void Game::handleKeyUp(const SDL_Keycode key) noexcept {
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

int Game::whichPlayer() {
	const auto playerId = m_board.peek(0x2067);	// player MSB
	switch (playerId) {
	case 0x21:
		return 1;
	case 0x22:
		return 2;
	default:
		return 0;
	}
}

int Game::drawFrame(int prior) {

	for (int y = 0; y < Board::RasterHeight; ++y) {

		if (y == 96)
			m_board.triggerInterruptScanLine96();

		prior = m_board.runScanLine(prior);
		drawScanLine(y);
	}

	m_board.triggerInterruptScanLine224();
	return m_board.runVerticalBlank(prior);
}

void Game::displayTexture() {

	const auto flip = m_configuration.getCocktailTable() && m_board.getCocktailModeControl();
	const auto flipped = SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL;
	const auto unflipped = SDL_FLIP_NONE;
	const SDL_RendererFlip effect = (SDL_RendererFlip)(flip ? flipped : unflipped);

	const auto gap = DisplayWidth - DisplayHeight;
	const auto correction = gap / 2 * DisplayScale;
	SDL_Rect destinationRectangle = { -correction, correction, DisplayWidth * DisplayScale, DisplayHeight * DisplayScale };

	verifySDLCall(::SDL_UpdateTexture(m_bitmapTexture.get(), nullptr, &m_pixels[0], DisplayWidth * sizeof(Uint32)), "Unable to update texture: ");
	verifySDLCall(
		::SDL_RenderCopyEx(
			m_renderer.get(),		// renderer
			m_bitmapTexture.get(),	// texture
			nullptr,				// srcrect
			&destinationRectangle,	// dstrect
			-90.0,					// angle
			nullptr,				// center
			effect),				// flip
		"Unable to copy texture to renderer");
	::SDL_RenderPresent(m_renderer.get());
}

void Game::drawScanLine(int y) {
	const auto black = m_colours.getColour(ColourPalette::Black);
	const auto bytesPerScanLine = DisplayWidth >> 3;
	auto address = bytesPerScanLine * y;
	for (int byteX = 0; byteX < bytesPerScanLine; ++byteX) {
		const auto video = m_board.VRAM().peek(address++);
		for (int bit = 0; bit < 8; ++bit) {
			const auto x = (byteX << 3) + bit;
			const auto mask = 1 << bit;
			const auto pixel = video & mask;
			const auto index = x + y * DisplayWidth;
			m_pixels[index] = pixel ? m_gel[index] : black;
		}
	}
}

void Game::Board_UfoSound(const EightBit::EventArgs&) {
	m_effects.playUfo();
}

void Game::Board_ShotSound(const EightBit::EventArgs&) {
	m_effects.playShot();
}

void Game::Board_PlayerDieSound(const EightBit::EventArgs&) {
	m_effects.playPlayerDie();
	auto controller = chooseController(whichPlayer());
	if (controller != nullptr)
		controller->startRumble();
}

void Game::Board_InvaderDieSound(const EightBit::EventArgs&) {
	m_effects.playInvaderDie();
}

void Game::Board_ExtendSound(const EightBit::EventArgs&) {
	m_effects.playExtend();
}

void Game::Board_Walk1Sound(const EightBit::EventArgs&) {
	m_effects.playWalk1();
}

void Game::Board_Walk2Sound(const EightBit::EventArgs&) {
	m_effects.playWalk2();
}

void Game::Board_Walk3Sound(const EightBit::EventArgs&) {
	m_effects.playWalk3();
}

void Game::Board_Walk4Sound(const EightBit::EventArgs&) {
	m_effects.playWalk4();
}

void Game::Board_UfoDieSound(const EightBit::EventArgs&) {
	m_effects.playUfoDie();
}

void Game::Board_EnableAmplifier(const EightBit::EventArgs&) {
	m_effects.enable();
}

void Game::Board_DisableAmplifier(const EightBit::EventArgs&) {
	m_effects.disable();
}
