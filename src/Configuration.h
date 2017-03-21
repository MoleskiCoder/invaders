#pragma once

#include <string>

class Configuration {
public:
	enum MachineMode {
		Unknown,
		SpaceInvaders,
		CPM
	};

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

	int getCyclesDuringRasterScan() const {
		return getCyclesPerFrame() - getCyclesDuringVerticalBlank();
	}

	int getCyclesDuringVerticalBlank() const {
		return getCyclesPerFrame() / 6;
	}

	int getCyclesPerSecond() const {
		return m_cyclesPerSecond;
	}

	void setCyclesPerSecond(int value) {
		m_cyclesPerSecond = value;
	}

	std::string getRomDirectory() const {
		return m_romDirectory;
	}

	MachineMode getMachineMode() const {
		return m_machineMode;
	}

	void setMachineMode(MachineMode value) {
		m_machineMode = value;
	}

	uint16_t getStartAddress() const {
		return m_startAddress;
	}

	void setStartAddress(uint16_t value) {
		m_startAddress = value;
	}

private:
	bool m_debugMode;
	bool m_profileMode;
	bool m_drawGraphics;
	bool m_showWatchdogOutput;

	bool m_vsyncLocked;
	int m_framesPerSecond;
	int m_cyclesPerSecond;

	std::string m_romDirectory;

	MachineMode m_machineMode;
	uint16_t m_startAddress;
};
