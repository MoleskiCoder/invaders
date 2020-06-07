#include "stdafx.h"
#include "Game.h"

Game::Game(const Configuration& configuration)
:	m_configuration(configuration),
	m_board(configuration),
	m_effects(configuration) {
}

void Game::raisePOWER() {

	Gaming::Game::raisePOWER();

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

	m_colours.load(pixelFormat().get());

	createGelPixels();

	m_board.raisePOWER();
}

void Game::createGelPixels() {
	for (int y = 0; y < rasterHeight(); ++y) {
		for (int x = 0; x < rasterWidth(); ++x) {
			const auto colourIndex = ColourPalette::calculateColour(y, rasterWidth() - x - 1);
			m_gel[x + (y * rasterWidth())] = m_colours.getColour(colourIndex);
		}
	}
}

const uint32_t* Game::pixels() const noexcept {
	return m_pixels.data();
}

bool Game::handleJoyButtonDown(const SDL_JoyButtonEvent event) {

	const auto handled = Gaming::Game::handleJoyButtonDown(event);
	if (handled)
		return true;

	const auto joystickId = event.which;
	const auto controllerIndex = mappedController(joystickId);
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
		default:
			return false;
		}
	}
	return true;
}

bool Game::handleJoyButtonUp(const SDL_JoyButtonEvent event) {

	const auto handled = Gaming::Game::handleJoyButtonUp(event);
	if (handled)
		return true;

	const auto joystickId = event.which;
	const auto controllerIndex = mappedController(joystickId);
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
		default:
			return false;
		}
	}
	return true;
}

bool Game::handleKeyDown(const SDL_Keycode key) {
	const auto handled = Gaming::Game::handleKeyDown(key);
	if (handled)
		return true;
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
	default:
		return false;
	}
	return true;
}

bool Game::handleKeyUp(const SDL_Keycode key) {
	const auto handled = Gaming::Game::handleKeyUp(key);
	if (handled)
		return true;
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
	default:
		return false;
	}
	return true;
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

void Game::runRasterLines() {
	m_board.runVerticalBlank();
	for (int y = 0; y < Board::RasterHeight; ++y) {

		if (y == 96)
			m_board.triggerInterruptScanLine96();

		m_board.runScanLine();
		drawScanLine(y);
	}

	m_board.triggerInterruptScanLine224();
}

void Game::copyTexture() {

	const auto flip = m_configuration.getCocktailTable() && m_board.getCocktailModeControl();
	constexpr auto flipped = SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL;
	constexpr auto unflipped = SDL_FLIP_NONE;
	const SDL_RendererFlip effect = (SDL_RendererFlip)(flip ? flipped : unflipped);

	const auto gap = displayWidth() - displayHeight();
	const auto correction = gap / 2 * displayScale();
	SDL_Rect destinationRectangle = { -correction, correction, displayWidth() * displayScale(), displayHeight() * displayScale() };

	Gaming::SDLWrapper::verifySDLCall(
		::SDL_RenderCopyEx(
			renderer().get(),		// renderer
			bitmapTexture().get(),	// texture
			nullptr,				// srcrect
			&destinationRectangle,	// dstrect
			-90.0,					// angle
			nullptr,				// center
			effect),				// flip
		"Unable to copy texture to renderer");
}

void Game::drawScanLine(int y) {
	const auto black = m_colours.getColour(ColourPalette::Black);
	const auto bytesPerScanLine = rasterWidth() >> 3;
	auto address = bytesPerScanLine * y;
	for (int byteX = 0; byteX < bytesPerScanLine; ++byteX) {
		const auto video = m_board.VRAM().peek(address++);
		for (int bit = 0; bit < 8; ++bit) {
			const auto x = (byteX << 3) + bit;
			const auto mask = 1 << bit;
			const auto pixel = video & mask;
			const auto index = x + y * rasterWidth();
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
