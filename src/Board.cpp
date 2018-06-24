#include "stdafx.h"
#include "Board.h"

#include <iostream>

Board::Board(const Configuration& configuration)
: m_configuration(configuration),
  m_cpu(EightBit::Intel8080(*this, m_ports)) {}

void Board::initialise() {

	auto romDirectory = m_configuration.getRomDirectory();

	m_romE.load(romDirectory + "/invaders.e");
	m_romF.load(romDirectory + "/invaders.f");
	m_romG.load(romDirectory + "/invaders.g");
	m_romH.load(romDirectory + "/invaders.h");

	m_ports.WritingPort.connect(std::bind(&Board::Board_PortWriting_SpaceInvaders, this, std::placeholders::_1));
	m_ports.WrittenPort.connect(std::bind(&Board::Board_PortWritten_SpaceInvaders, this, std::placeholders::_1));
	m_ports.ReadingPort.connect(std::bind(&Board::Board_PortReading_SpaceInvaders, this, std::placeholders::_1));

	if (m_configuration.isProfileMode()) {
		CPU().ExecutingInstruction.connect(std::bind(&Board::Cpu_ExecutingInstruction_Profile, this, std::placeholders::_1));
	}

	if (m_configuration.isDebugMode()) {
		CPU().ExecutingInstruction.connect(std::bind(&Board::Cpu_ExecutingInstruction_Debug, this, std::placeholders::_1));
	}

	CPU().powerOn();
	CPU().PC() = m_configuration.getStartAddress();
}

void Board::Board_PortWriting_SpaceInvaders(const uint8_t& port) {
	auto value = m_ports.readOutputPort(port);
	switch (port) {
	case SOUND1:
		m_preSound1 = value;
		break;
	case SOUND2:
		m_preSound2 = value;
		break;
	}
}

void Board::Board_PortWritten_SpaceInvaders(const uint8_t& port) {
	auto value = m_ports.readOutputPort(port);
	switch (port) {
	case SHFTAMNT:
		m_shiftAmount = value & EightBit::Processor::Mask3;
		break;
	case SHFT_DATA:
		m_shiftData.low = m_shiftData.high;
		m_shiftData.high = value;
		break;
	case WATCHDOG:
		if (m_configuration.isShowWatchdogOutput())
			std::cout << (value < 64 ? m_characterSet[value] : '_');
		break;
	case SOUND1: {
			auto soundUfo = ((value & 1) != 0);
			if (soundUfo)
				UfoSound.fire(EightBit::EventArgs::empty());

			auto soundShot = ((value & 2) != 0) && ((m_preSound1 & 2) == 0);
			if (soundShot)
				ShotSound.fire(EightBit::EventArgs::empty());

			auto soundPlayerDie = ((value & 4) != 0) && ((m_preSound1 & 4) == 0);
			if (soundPlayerDie)
				PlayerDieSound.fire(EightBit::EventArgs::empty());
	
			auto soundInvaderDie = ((value & 8) != 0) && ((m_preSound1 & 8) == 0);
			if (soundInvaderDie)
				InvaderDieSound.fire(EightBit::EventArgs::empty());

			auto extend = ((value & 0x10) != 0) && ((m_preSound1 & 0x10) == 0);
			if (extend)
				ExtendSound.fire(EightBit::EventArgs::empty());

			auto ampenable = ((value & 0x20) != 0) && ((m_preSound1 & 0x20) == 0);
			if (ampenable)
				EnableAmplifier.fire(EightBit::EventArgs::empty());

			auto ampdisable = ((value & 0x20) == 0) && ((m_preSound1 & 0x20) != 0);
			if (ampdisable)
				DisableAmplifier.fire(EightBit::EventArgs::empty());
		}
		break;

	case SOUND2: {
			auto soundWalk1 = ((value & 1) != 0) && ((m_preSound2 & 1) == 0);
			if (soundWalk1)
				Walk1Sound.fire(EightBit::EventArgs::empty());

			auto soundWalk2 = ((value & 2) != 0) && ((m_preSound2 & 2) == 0);
			if (soundWalk2)
				Walk2Sound.fire(EightBit::EventArgs::empty());

			auto soundWalk3 = ((value & 4) != 0) && ((m_preSound2 & 4) == 0);
			if (soundWalk3)
				Walk3Sound.fire(EightBit::EventArgs::empty());

			auto soundWalk4 = ((value & 8) != 0) && ((m_preSound2 & 8) == 0);
			if (soundWalk4)
				Walk4Sound.fire(EightBit::EventArgs::empty());

			auto soundUfoDie = ((value & 0x10) != 0) && ((m_preSound2 & 0x10) == 0);
			if (soundUfoDie)
				UfoDieSound.fire(EightBit::EventArgs::empty());

			m_cocktailModeControl = (value & 0x20) != 0;
		}
		break;
	}
}

void Board::Board_PortReading_SpaceInvaders(const uint8_t& port) {
	switch (port) {
	case INP1:
		m_ports.writeInputPort(port,
			((int)!m_credit)
			| (m_twoPlayerStart << 1)
			| (m_onePlayerStart << 2)
			| (1 << 3)
			| (m_onePlayerShot << 4)
			| (m_onePlayerLeft << 5)
			| (m_onePlayerRight << 6)
		);
		break;
	case INP2:
		m_ports.writeInputPort(port,
			m_ships
			| (m_tilt << 2)
			| (m_extraLife << 3)
			| (m_twoPlayerShot << 4)
			| (m_twoPlayerLeft << 5)
			| (m_twoPlayerRight << 6)
			| (m_demoCoinInfo << 7)
		);
		break;
	case SHFT_IN:
		m_ports.writeInputPort(port,
			((((m_shiftData.high << 8) | m_shiftData.low) << m_shiftAmount) >> 8));
		break;
	}
}

void Board::Cpu_ExecutingInstruction_Profile(const EightBit::Intel8080& cpu) {

	const auto pc = CPU().PC();

	m_profiler.addAddress(pc.word);
	m_profiler.addInstruction(peek(pc.word));
}

void Board::Cpu_ExecutingInstruction_Debug(const EightBit::Intel8080&) {

	std::cerr
		<< EightBit::Disassembler::state(CPU())
		<< "\t"
		<< m_disassembler.disassemble(CPU())
		<< '\n';
}

uint8_t& Board::reference(uint16_t address) {
	address &= ~0xc000;
	if (address < 0x2000) {
		if (address < 0x800)
			return DATA() = m_romH.peek(address);
		if (address < 0x1000)
			return DATA() = m_romG.peek(address - 0x800);
		if (address < 0x1800)
			return DATA() = m_romF.peek(address - (0x800 * 2));
		return DATA() = m_romE.peek(address - (0x800 * 3));
	}
	if (address < 0x4000) {
		if (address < 0x2400)
			return m_workRAM.reference(address - 0x2000);
		return m_videoRAM.reference(address - 0x2400);
	}
	UNREACHABLE;
}
