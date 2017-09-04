#pragma once

#include <string>

#include <Memory.h>
#include <InputOutput.h>
#include <Intel8080.h>
#include <Profiler.h>
#include <EventArgs.h>
#include <Disassembler.h>

#include "Configuration.h"

class Board {
public:
	enum {
		RasterWidth = 256,
		RasterHeight = 224
	};

	enum {
		WorkRam = 0x2000,
		VideoRam = 0x2400
	};

	Board(const Configuration& configuration);

	EightBit::Profiler& Profiler() { return m_profiler; }
	EightBit::Memory& Bus() { return m_memory; }
	EightBit::Intel8080& CPU() { return m_cpu; }

	void initialise();

	int triggerInterruptScanLine224() {
		return m_cpu.interrupt(0xd7);	// RST 2
	}

	int triggerInterruptScanLine96() {
		return m_cpu.interrupt(0xcf);	// RST 1
	}

	bool getCocktailModeControl() const {
		return m_cocktailModeControl;
	}

	int getCyclesPerScanLine() const {
		return m_configuration.getCyclesPerRasterScan() / RasterHeight;
	}

	int runFrame(int prior) {
		prior = runRasterScan(prior);
		return runVerticalBlank(prior);
	}

	int runScanLine(int prior) {
		return m_cpu.run(getCyclesPerScanLine() - prior);
	}

	int runRasterScan(int prior) {
		return m_cpu.run(m_configuration.getCyclesPerRasterScan() - prior);
	}

	int runVerticalBlank(int prior) {
		return m_cpu.run(m_configuration.getCyclesPerVerticalBlank() - prior);
	}

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

	EightBit::Signal<EightBit::EventArgs> UfoSound;
	EightBit::Signal<EightBit::EventArgs> ShotSound;
	EightBit::Signal<EightBit::EventArgs> PlayerDieSound;
	EightBit::Signal<EightBit::EventArgs> InvaderDieSound;
	EightBit::Signal<EightBit::EventArgs> ExtendSound;

	EightBit::Signal<EightBit::EventArgs> Walk1Sound;
	EightBit::Signal<EightBit::EventArgs> Walk2Sound;
	EightBit::Signal<EightBit::EventArgs> Walk3Sound;
	EightBit::Signal<EightBit::EventArgs> Walk4Sound;
	EightBit::Signal<EightBit::EventArgs> UfoDieSound;

	EightBit::Signal<EightBit::EventArgs> EnableAmplifier;
	EightBit::Signal<EightBit::EventArgs> DisableAmplifier;

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
	EightBit::Memory m_memory;
	EightBit::InputOutput m_ports;
	EightBit::Intel8080 m_cpu;
	EightBit::Profiler m_profiler;
	EightBit::Disassembler m_disassembler;

	ShipSwitch m_ships;
	ExtraShipSwitch m_extraLife;
	DemoCoinInfoSwitch m_demoCoinInfo;

	uint8_t m_shiftAmount;
	EightBit::register16_t m_shiftData;

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

	bool m_cocktailModeControl;

	void Board_PortWriting_SpaceInvaders(const EightBit::PortEventArgs& portEvent);
	void Board_PortWritten_SpaceInvaders(const EightBit::PortEventArgs& portEvent);
	void Board_PortReading_SpaceInvaders(const EightBit::PortEventArgs& portEvent);

	void Cpu_ExecutingInstruction_Debug(const EightBit::Intel8080& cpuEvent);
	void Cpu_ExecutingInstruction_Profile(const EightBit::Intel8080& cpuEvent);
};
