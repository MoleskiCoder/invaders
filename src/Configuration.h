#pragma once

#include <string>

#include <Register.h>

class Configuration final {
public:
	Configuration() = default;

	bool isDebugMode() const { return m_debugMode; }
	void setDebugMode(bool value) { m_debugMode = value; }

	bool isProfileMode() const { return m_profileMode; }
	void setProfileMode(bool value) { m_profileMode = value; }

	bool isDrawGraphics() const { return m_drawGraphics; }
	void setDrawGraphics(bool value) { m_drawGraphics = value; }

	bool isShowWatchdogOutput() const { return m_showWatchdogOutput; }
	void setShowWatchdogOutput(bool value) { m_showWatchdogOutput = value; }

	bool isInterlaced() const { return m_interlaced; }
	void setInterlaced(bool value) { m_interlaced = value; }

	bool getVsyncLocked() const { return m_vsyncLocked;	}
	void setVsyncLocked(bool value) { m_vsyncLocked = value; }

	int getFramesPerSecond() const { return m_framesPerSecond; }
	void setFramesPerSecond(int value) { m_framesPerSecond = value; }

	int getCyclesPerFrame() const { return m_cyclesPerSecond / getFramesPerSecond(); }
	int getCyclesPerRasterScan() const { return getCyclesPerFrame() - getCyclesPerVerticalBlank(); }
	int getCyclesPerVerticalBlank() const { return getCyclesPerFrame() / 6; }

	int getCyclesPerSecond() const { return m_cyclesPerSecond; }
	void setCyclesPerSecond(int value) { m_cyclesPerSecond = value; }

	bool getCocktailTable() const { return m_cocktailTable; }
	void setCocktailTable(bool value) { m_cocktailTable = value; }

	std::string getRomDirectory() const { return m_romDirectory; }
	std::string getSoundDirectory() const { return m_soundDirectory; }

	static EightBit::register16_t getStartAddress() {
		return 0U;
	}

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
