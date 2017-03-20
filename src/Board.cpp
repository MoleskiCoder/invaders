#include "stdafx.h"
#include "Board.h"
#include "Disassembler.h"

Board::Board(const Configuration& configuration)
: m_configuration(configuration),
  m_cpu(Intel8080(m_memory, m_ports)),
  m_ships(Three),
  m_extraLife(OneThousandFiveHundred),
  m_demoCoinInfo(On),
  m_shiftAmount(0),
  m_shiftData(0),
  m_credit(true),
  m_onePlayerStart(false),
  m_onePlayerShot(false),
  m_onePlayerLeft(false),
  m_onePlayerRight(false),
  m_twoPlayerStart(false),
  m_twoPlayerShot(false),
  m_twoPlayerLeft(false),
  m_twoPlayerRight(false),
  m_tilt(false) {
}

void Board::initialise() {

	m_memory.clear();
	auto romDirectory = m_configuration.getRomDirectory();

	switch (m_configuration.getMachineMode()) {
	case Configuration::SpaceInvaders:

		m_memory.loadRom(romDirectory + "/invaders.e", 0x1800);
		m_memory.loadRom(romDirectory + "/invaders.f", 0x1000);
		m_memory.loadRom(romDirectory + "/invaders.g", 0x0800);
		m_memory.loadRom(romDirectory + "/invaders.h", 0x0000);

		m_ports.WrittenPort.connect(std::bind(&Board::Board_PortWritten_SpaceInvaders, this, std::placeholders::_1));
		m_ports.ReadingPort.connect(std::bind(&Board::Board_PortReading_SpaceInvaders, this, std::placeholders::_1));
		break;

	case Configuration::CPM:
		//m_memory.loadRom(romDirectory + "/TEST.COM", 0x100);		// Microcosm
		//m_memory.loadRom(romDirectory + "/8080PRE.COM", 0x100);	// Bartholomew preliminary
		m_memory.loadRom(romDirectory + "/8080EX1.COM", 0x100);	// Cringle/Bartholomew
		//m_memory.loadRom(romDirectory + "/CPUTEST.COM", 0x100);	// SuperSoft diagnostics

		m_memory.set(5, 0xc9);	// ret
		m_cpu.ExecutingInstruction.connect(std::bind(&Board::Cpu_ExecutingInstruction_Cpm, this, std::placeholders::_1));
		break;
	}

	if (m_configuration.isProfileMode()) {
		m_cpu.ExecutingInstruction.connect(std::bind(&Board::Cpu_ExecutingInstruction_Profile, this, std::placeholders::_1));
	}

	if (m_configuration.isDebugMode()) {
		m_cpu.ExecutingInstruction.connect(std::bind(&Board::Cpu_ExecutingInstruction_Debug, this, std::placeholders::_1));
	}

	m_cpu.initialise();
	m_cpu.setProgramCounter(m_configuration.getStartAddress());
}

void Board::Cpu_ExecutingInstruction_Cpm(const CpuEventArgs& cpuEvent) {
	auto pc = m_cpu.getProgramCounter();
	switch (pc) {
	case 0x0:	// CP/M warm start
		m_cpu.halt();
		m_profiler.dump();
		break;
	case 0x5:	// BDOS
		bdos();
		break;
	default:
		break;
	}
}

void Board::bdos() {
	auto c = m_cpu.getC();
	switch (c) {
	case 0x2: {
		auto character = m_cpu.getE();
		std::cout << character;
		break;
	}
	case 0x9:
		auto de = Memory::makeWord(m_cpu.getE(), m_cpu.getD());
		for (uint16_t i = de; m_memory.get(i) != '$'; ++i) {
			std::cout << m_memory.get(i);
		}
		break;
	}
}

void Board::Board_PortWritten_SpaceInvaders(const PortEventArgs& portEvent) {
	auto port = portEvent.getPort();
	auto value = m_ports.readOutputPort(port);
	switch (port) {
	case SHFTAMNT:
		m_shiftAmount = value & 0x7;
		break;
	case SHFT_DATA:
		m_shiftData = value;
		break;
	case WATCHDOG:
		std::cout << (value < 42 ? m_characterSet[value] : '_');
		break;
	}
}

void Board::Board_PortReading_SpaceInvaders(const PortEventArgs& portEvent) {
	auto port = portEvent.getPort();
	switch (port) {
	case INP1:
		m_ports.writeInputPort(port,
			(int)m_credit
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
			(m_shiftData << m_shiftAmount) >> 8);
		break;
	}
}

void Board::Cpu_ExecutingInstruction_Profile(const CpuEventArgs& cpuEvent) {

	const auto& cpu = cpuEvent.getCpu();
	const auto pc = cpu.getProgramCounter();

	m_profiler.addAddress(pc);
	m_profiler.addInstruction(m_memory.get(pc));
}

void Board::Cpu_ExecutingInstruction_Debug(const CpuEventArgs& cpuEvent) {

	const auto& cpu = cpuEvent.getCpu();
	const auto pc = cpu.getProgramCounter();
	const auto instruction = cpu.getMemory().get(pc);

	std::cerr
		<< Disassembler::state(cpuEvent.getCpu())
		<< "\t"
		<< Disassembler::disassemble(cpuEvent.getCpu())
		<< '\n';
}