#pragma once

#include <string>

#include <Register.h>

class Configuration final {
public:
	Configuration() = default;

	auto isDebugMode() const { return m_debugMode; }
	auto setDebugMode(const bool value) { m_debugMode = value; }

	auto isProfileMode() const { return m_profileMode; }
	void setProfileMode(const bool value) { m_profileMode = value; }

	bool isDrawGraphics() const { return m_drawGraphics; }
	void setDrawGraphics(const bool value) { m_drawGraphics = value; }

	auto isShowWatchdogOutput() const { return m_showWatchdogOutput; }
	void setShowWatchdogOutput(const bool value) { m_showWatchdogOutput = value; }

	auto isInterlaced() const { return m_interlaced; }
	void setInterlaced(const bool value) { m_interlaced = value; }

	auto getVsyncLocked() const { return m_vsyncLocked;	}
	void setVsyncLocked(const bool value) { m_vsyncLocked = value; }

	auto getFramesPerSecond() const { return m_framesPerSecond; }
	void setFramesPerSecond(const int value) { m_framesPerSecond = value; }

	auto getCyclesPerFrame() const { return m_cyclesPerSecond / getFramesPerSecond(); }
	auto getCyclesPerVerticalBlank() const { return getCyclesPerFrame() / 6; }
	auto getCyclesPerRasterScan() const { return getCyclesPerFrame() - getCyclesPerVerticalBlank(); }

	int getCyclesPerSecond() const { return m_cyclesPerSecond; }
	void setCyclesPerSecond(const int value) { m_cyclesPerSecond = value; }

	auto getCocktailTable() const { return m_cocktailTable; }
	void setCocktailTable(const bool value) { m_cocktailTable = value; }

	auto getRomDirectory() const { return m_romDirectory; }
	auto getSoundDirectory() const { return m_soundDirectory; }

private:
	bool m_debugMode = false;
	bool m_profileMode = false;
	bool m_drawGraphics = true;
	bool m_showWatchdogOutput = false;

	bool m_interlaced = false;
	bool m_vsyncLocked = true;
	int m_framesPerSecond = 60;
	int m_cyclesPerSecond = 2000000;
	bool m_cocktailTable = false;

	std::string m_romDirectory = "roms";
	std::string m_soundDirectory = "sounds";
};
