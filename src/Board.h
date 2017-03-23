#pragma once

#include <string>

#include "Memory.h"
#include "InputOutput.h"
#include "Configuration.h"
#include "Intel8080.h"
#include "Profiler.h"
#include "EventArgs.h"

class Board {
public:
	enum {
		RasterWidth = 256,
		RasterHeight = 224
	};

	Board(const Configuration& configuration);

	Memory& getMemory() { return m_memory; }
	const Intel8080& getCPU() const { return m_cpu; }
	Intel8080& getCPUMutable() { return m_cpu; }

	void initialise();

	void pressCredit() { m_credit = true; }
	void releaseCredit() { m_credit = false; }

	void press1P() { m_onePlayerStart = true; }
	void pressShoot1P() { m_onePlayerShot = true; }
	void pressLeft1P() { m_onePlayerLeft = true; }
	void pressRight1P() { m_onePlayerRight = true; }

	void release1P() { m_onePlayerStart = false; }
	void releaseShoot1P() { m_onePlayerShot = false; }
	void releaseLeft1P() { m_onePlayerLeft = false; }
	void releaseRight1P() { m_onePlayerRight = false; }

	void press2P() { m_twoPlayerStart = true; }
	void pressShoot2P() { m_twoPlayerShot = true; }
	void pressLeft2P() { m_twoPlayerLeft = true; }
	void pressRight2P() { m_twoPlayerRight = true; }

	void release2P() { m_twoPlayerStart = false; }
	void releaseShoot2P() { m_twoPlayerShot = false; }
	void releaseLeft2P() { m_twoPlayerLeft = false; }
	void releaseRight2P() { m_twoPlayerRight = false; }

	Signal<EventArgs> UfoSound;
	Signal<EventArgs> ShotSound;
	Signal<EventArgs> PlayerDieSound;
	Signal<EventArgs> InvaderDieSound;

	Signal<EventArgs> Walk1Sound;
	Signal<EventArgs> Walk2Sound;
	Signal<EventArgs> Walk3Sound;
	Signal<EventArgs> Walk4Sound;
	Signal<EventArgs> UfoDieSound;

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

	std::array<char, 64> m_characterSet = { {
		'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
		'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
		'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
		'Y', 'Z', '0', '1', '2', '3', '4', '5',
		'6', '7', '8', '9', '<', '>', ' ', '=',
		'*', '^', '_', '_', '_', '_', '_', '_',
		'Y', '%', '_', '_', '_', '_', 'Y', '&',
		'?', '_', '_', '_', '_', '_', '_', '-',
	} };

	const Configuration& m_configuration;
	Memory m_memory;
	InputOutput m_ports;
	Intel8080 m_cpu;
	Profiler m_profiler;

	ShipSwitch m_ships;
	ExtraShipSwitch m_extraLife;
	DemoCoinInfoSwitch m_demoCoinInfo;

	uint8_t m_shiftAmount;
	uint8_t m_shiftDataLow;
	uint8_t m_shiftDataHigh;

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

	uint8_t m_preSound1;
	uint8_t m_preSound2;

	void Cpu_ExecutingInstruction_Cpm(const CpuEventArgs& cpuEvent);

	void Board_PortWriting_SpaceInvaders(const PortEventArgs& portEvent);
	void Board_PortWritten_SpaceInvaders(const PortEventArgs& portEvent);
	void Board_PortReading_SpaceInvaders(const PortEventArgs& portEvent);

	void Cpu_ExecutingInstruction_Debug(const CpuEventArgs& cpuEvent);
	void Cpu_ExecutingInstruction_Profile(const CpuEventArgs& cpuEvent);

	void bdos();
};
