#pragma once

#include <string>

class Configuration final {
public:
	Configuration() = default;

	auto isDebugMode() const noexcept { return m_debugMode; }
	auto setDebugMode(const bool value) noexcept { m_debugMode = value; }

	auto isShowWatchdogOutput() const noexcept { return m_showWatchdogOutput; }
	void setShowWatchdogOutput(const bool value) noexcept { m_showWatchdogOutput = value; }

	auto getVsyncLocked() const noexcept { return m_vsyncLocked;	}
	void setVsyncLocked(const bool value) noexcept { m_vsyncLocked = value; }

	auto getFramesPerSecond() const noexcept { return m_framesPerSecond; }
	void setFramesPerSecond(const int value) noexcept { m_framesPerSecond = value; }

	auto getCyclesPerFrame() const noexcept { return m_cyclesPerSecond / getFramesPerSecond(); }
	auto getCyclesPerVerticalBlank() const noexcept { return getCyclesPerFrame() / 6; }
	auto getCyclesPerRasterScan() const noexcept { return getCyclesPerFrame() - getCyclesPerVerticalBlank(); }

	int getCyclesPerSecond() const noexcept { return m_cyclesPerSecond; }
	void setCyclesPerSecond(const int value) noexcept { m_cyclesPerSecond = value; }

	auto getCocktailTable() const noexcept { return m_cocktailTable; }
	void setCocktailTable(const bool value) noexcept { m_cocktailTable = value; }

	auto getRomDirectory() const noexcept { return m_romDirectory; }
	auto getSoundDirectory() const noexcept { return m_soundDirectory; }

private:
	bool m_debugMode = false;
	bool m_showWatchdogOutput = false;

	bool m_vsyncLocked = true;
	int m_framesPerSecond = 60;
	int m_cyclesPerSecond = 2000000;
	bool m_cocktailTable = false;

	std::string m_romDirectory = "roms";
	std::string m_soundDirectory = "sounds";
};
