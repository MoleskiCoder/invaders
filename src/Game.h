#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include <Game.h>
#include <GameController.h>

#include "Board.h"
#include "ColourPalette.h"
#include "SoundEffects.h"

class Configuration;
class Expansion;

class Game final : public Gaming::Game {
public:
	Game(const Configuration& configuration);
	~Game() { }

	virtual void raisePOWER() override;

	Board& BUS() noexcept { return m_board; }
	const Board& BUS() const noexcept { return m_board; }

protected:
	int fps() const noexcept final { return m_configuration.getFramesPerSecond(); }
	bool useVsync() const noexcept final { return m_configuration.getVsyncLocked(); }

	int windowWidth() const noexcept final { return displayHeight() * displayScale(); }
	int windowHeight() const noexcept final { return displayWidth() * displayScale(); }
	int displayWidth() const noexcept final { return rasterWidth(); }
	int displayHeight() const noexcept final { return rasterHeight(); }
	int displayScale() const noexcept final { return 2; }
	int rasterWidth() const noexcept final { return Board::RasterWidth; }
	int rasterHeight() const noexcept final { return Board::RasterHeight; }

	std::string title() const final { return "Space Invaders"; }

	const uint32_t* pixels() const noexcept final;

	void runFrame() final;

	void handleKeyDown(SDL_Keycode key) final;
	void handleKeyUp(SDL_Keycode key) final;

	void handleJoyButtonDown(SDL_JoyButtonEvent event) final;
	void handleJoyButtonUp(SDL_JoyButtonEvent event) final;

	void copyTexture() final;

private:
	std::array<uint32_t, Board::RasterWidth * Board::RasterHeight> m_pixels;
	std::array<uint32_t, Board::RasterWidth * Board::RasterHeight> m_gel;

	const Configuration& m_configuration;
	ColourPalette m_colours;
	Board m_board;
	SoundEffects m_effects;


	void createGelPixels();

	int whichPlayer();

	void drawScanLine(int y);

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
};
