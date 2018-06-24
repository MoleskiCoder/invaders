#pragma once

#include <string>

#include <Rom.h>
#include <Ram.h>
#include <Bus.h>
#include <InputOutput.h>
#include <Intel8080.h>
#include <Profiler.h>
#include <EventArgs.h>
#include <Disassembler.h>

#include "Configuration.h"

class Board final : public EightBit::Bus {
public:
	enum {
		RasterWidth = 256,
		RasterHeight = 224
	};

	Board(const Configuration& configuration);

	auto& Profiler() { return m_profiler; }
	auto& CPU() { return m_cpu; }

	auto& VRAM() { return m_videoRAM; }

	void initialise();

	void triggerInterruptScanLine224() {
		DATA() = 0xd7;	// RST 2
		EightBit::Processor::lower(CPU().INT());
	}

	void triggerInterruptScanLine96() {
		DATA() = 0xcf;	// RST 1
		EightBit::Processor::lower(CPU().INT());
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

protected:
	virtual uint8_t& reference(uint16_t address) final;

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

	EightBit::Rom m_romE = 0x800;
	EightBit::Rom m_romF = 0x800;
	EightBit::Rom m_romG = 0x800;
	EightBit::Rom m_romH = 0x800;
	EightBit::Ram m_workRAM = 0x400;
	EightBit::Ram m_videoRAM = 0x1c00;

	EightBit::InputOutput m_ports;
	EightBit::Intel8080 m_cpu;
	EightBit::Profiler m_profiler;
	EightBit::Disassembler m_disassembler;

	ShipSwitch m_ships = Three;
	ExtraShipSwitch m_extraLife = OneThousandFiveHundred;
	DemoCoinInfoSwitch m_demoCoinInfo = On;

	uint8_t m_shiftAmount = 0;
	EightBit::register16_t m_shiftData = 0U;

	bool m_credit = false;

	bool m_onePlayerStart = false;
	bool m_onePlayerShot = false;
	bool m_onePlayerLeft = false;
	bool m_onePlayerRight = false;

	bool m_twoPlayerStart = false;
	bool m_twoPlayerShot = false;
	bool m_twoPlayerLeft = false;
	bool m_twoPlayerRight = false;

	bool m_tilt = false;

	uint8_t m_preSound1 = 0;
	uint8_t m_preSound2 = 0;

	bool m_cocktailModeControl = false;

	void Board_PortWriting_SpaceInvaders(const uint8_t& port);
	void Board_PortWritten_SpaceInvaders(const uint8_t& port);
	void Board_PortReading_SpaceInvaders(const uint8_t& port);

	void Cpu_ExecutingInstruction_Debug(const EightBit::Intel8080& cpuEvent);
	void Cpu_ExecutingInstruction_Profile(const EightBit::Intel8080& cpuEvent);
};
