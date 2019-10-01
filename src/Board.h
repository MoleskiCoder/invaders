#pragma once

#include <array>
#include <cstdint>

#include <Bus.h>
#include <Disassembler.h>
#include <EventArgs.h>
#include <InputOutput.h>
#include <Intel8080.h>
#include <Profiler.h>
#include <Register.h>
#include <Ram.h>
#include <Rom.h>
#include <Signal.h>

#include "Configuration.h"

class Board final : public EightBit::Bus {
public:
	enum {
		RasterWidth = 256,
		RasterHeight = 224
	};

	Board(const Configuration& configuration);

	auto& CPU() noexcept { return m_cpu; }

	auto& VRAM() noexcept { return m_videoRAM; }

	void initialise() final;
	void raisePOWER() final;
	void lowerPOWER() final;

	void triggerInterruptScanLine224() noexcept {
		DATA() = 0xd7;	// RST 2
		EightBit::Chip::lower(CPU().INT());
	}

	void triggerInterruptScanLine96() noexcept {
		DATA() = 0xcf;	// RST 1
		EightBit::Chip::lower(CPU().INT());
	}

	auto getCocktailModeControl() const noexcept {
		return m_cocktailModeControl;
	}

	auto getCyclesPerScanLine() const noexcept {
		return m_configuration.getCyclesPerRasterScan() / RasterHeight;
	}

	auto runScanLine(const int prior) {
		return m_cpu.run(getCyclesPerScanLine() - prior);
	}

	auto runRasterScan(const int prior) {
		return m_cpu.run(m_configuration.getCyclesPerRasterScan() - prior);
	}

	auto runVerticalBlank(const int prior) {
		return m_cpu.run(m_configuration.getCyclesPerVerticalBlank() - prior);
	}

	auto runFrame(const int prior) {
		const auto remaining = runRasterScan(prior);
		return runVerticalBlank(remaining);
	}

	void pressCredit() noexcept { m_credit = true; }
	void releaseCredit() noexcept { m_credit = false; }

	void press1P() noexcept { m_onePlayerStart = true; }
	void pressShoot1P() noexcept { m_onePlayerShot = true; }
	void pressLeft1P() noexcept { m_onePlayerLeft = true; }
	void pressRight1P() noexcept { m_onePlayerRight = true; }

	void release1P() noexcept { m_onePlayerStart = false; }
	void releaseShoot1P() noexcept { m_onePlayerShot = false; }
	void releaseLeft1P() noexcept { m_onePlayerLeft = false; }
	void releaseRight1P() noexcept { m_onePlayerRight = false; }

	void press2P() noexcept { m_twoPlayerStart = true; }
	void pressShoot2P() noexcept { m_twoPlayerShot = true; }
	void pressLeft2P() noexcept { m_twoPlayerLeft = true; }
	void pressRight2P() noexcept { m_twoPlayerRight = true; }

	void release2P() noexcept { m_twoPlayerStart = false; }
	void releaseShoot2P() noexcept { m_twoPlayerShot = false; }
	void releaseLeft2P() noexcept { m_twoPlayerLeft = false; }
	void releaseRight2P() noexcept { m_twoPlayerRight = false; }

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
	virtual EightBit::MemoryMapping mapping(uint16_t address) final;

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
	EightBit::Disassembler m_disassembler;

	const ShipSwitch m_ships = Three;
	const ExtraShipSwitch m_extraLife = OneThousandFiveHundred;
	const DemoCoinInfoSwitch m_demoCoinInfo = On;

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
};
