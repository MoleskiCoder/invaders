#pragma once

#include <string>

#include "Memory.h"
#include "InputOutput.h"
#include "Configuration.h"
#include "Intel8080.h"
#include "Profiler.h"

class Board {
public:
	Board(const Configuration& configuration);

	const Configuration& getConfiguration() const { return m_configuration; }
	Memory& getMemory() { return m_memory; }
	const Intel8080& getCPU() const { return m_cpu; }
	Intel8080& getCPUMutable() { return m_cpu; }
	InputOutput& getIO() { return m_ports; }

	void initialise();

private:
	enum InputPorts {
		INP0 = 0,
		INP1 = 1,
		INP2 = 2,
		SHFT_IN = 3
	};

	enum OutputPorts {
		SHFTAMNT = 2,
		SOUND1 = 3,
		SHFT_DATA = 4,
		SOUND2 = 5,
		WATCHDOG = 6
	};

	enum ShipSwitch {
		Three = 0b00,
		Four = 0b01,
		Five = 0b10,
		Six = 0b11,
	};

	enum ExtraShipSwitch {
		OneThousandFiveHundred = 0,
		OneThousand = 1,
	};

	enum DemoCoinInfoSwitch {
		On = 0,
		Off = 1,
	};

	std::array<char, 42> m_characterSet = {
		'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
		'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
		'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
		'Y', 'Z', '0', '1', '2', '3', '4', '5',
		'6', '7', '8', '9', '<', '>', ' ', '=',
		'*', '^'
	};

	const Configuration& m_configuration;
	Memory m_memory;
	InputOutput m_ports;
	Intel8080 m_cpu;
	Profiler m_profiler;

	ShipSwitch m_ships;
	ExtraShipSwitch m_extraLife;
	DemoCoinInfoSwitch m_demoCoinInfo;

	uint8_t m_shiftAmount;
	uint8_t m_shiftData;

	bool m_credit;

	bool m_onePlayerStart;
	bool m_onePlayerShot;
	bool m_onePlayerLeft;
	bool m_onePlayerRight;

	bool m_twoPlayerStart;
	bool m_twoPlayerShot;
	bool m_twoPlayerLeft;
	bool m_twoPlayerRight;

	bool m_tilt;

	void Cpu_ExecutingInstruction_Cpm(const CpuEventArgs& cpuEvent);

	void Board_PortWritten_SpaceInvaders(const PortEventArgs& portEvent);
	void Board_PortReading_SpaceInvaders(const PortEventArgs& portEvent);

	void Cpu_ExecutingInstruction_Debug(const CpuEventArgs& cpuEvent);

	void bdos();
};
