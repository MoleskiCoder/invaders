#pragma once

#include <string>

#include <Memory.h>

class Configuration {
public:
	Configuration();

	bool isDebugMode() const {
		return m_debugMode;
	}

	void setDebugMode(bool value) {
		m_debugMode = value;
	}

	bool isProfileMode() const {
		return m_profileMode;
	}

	void setProfileMode(bool value) {
		m_profileMode = value;
	}

	bool isDrawGraphics() const {
		return m_drawGraphics;
	}

	void setDrawGraphics(bool value) {
		m_drawGraphics = value;
	}

	bool isShowWatchdogOutput() const {
		return m_showWatchdogOutput;
	}

	void setShowWatchdogOutput(bool value) {
		m_showWatchdogOutput = value;
	}

	bool isInterlaced() const {
		return m_interlaced;
	}

	void setInterlaced(bool value) {
		m_interlaced = value;
	}

	bool getVsyncLocked() const {
		return m_vsyncLocked;
	}

	void setVsyncLocked(bool value) {
		m_vsyncLocked = value;
	}

	int getFramesPerSecond() const {
		return m_framesPerSecond;
	}

	void setFramesPerSecond(int value) {
		m_framesPerSecond = value;
	}

	int getCyclesPerFrame() const {
		return m_cyclesPerSecond / getFramesPerSecond();
	}

	int getCyclesPerRasterScan() const {
		return getCyclesPerFrame() - getCyclesPerVerticalBlank();
	}

	int getCyclesPerVerticalBlank() const {
		return getCyclesPerFrame() / 6;
	}

	int getCyclesPerSecond() const {
		return m_cyclesPerSecond;
	}

	void setCyclesPerSecond(int value) {
		m_cyclesPerSecond = value;
	}

	bool getCocktailTable() const {
		return m_cocktailTable;
	}

	void getCocktailTable(bool value) {
		m_cocktailTable = value;
	}

	std::string getRomDirectory() const {
		return m_romDirectory;
	}

	std::string getSoundDirectory() const {
		return m_soundDirectory;
	}

	EightBit::register16_t getStartAddress() const {
		EightBit::register16_t returned;
		returned.word = 0;
		return returned;
	}

private:
	bool m_debugMode;
	bool m_profileMode;
	bool m_drawGraphics;
	bool m_showWatchdogOutput;

	bool m_interlaced;
	bool m_vsyncLocked;
	int m_framesPerSecond;
	int m_cyclesPerSecond;
	bool m_cocktailTable;

	std::string m_romDirectory;
	std::string m_soundDirectory;
};
