#include "stdafx.h"
#include "Board.h"
#include "Disassembler.h"

Board::Board(const Configuration& configuration)
: m_configuration(configuration),
  m_cpu(Intel8080(m_memory, m_ports)) {
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

	m_ports.WrittenPort.connect(std::bind(&Board::Board_PortWritten, this, std::placeholders::_1));
	m_ports.ReadPort.connect(std::bind(&Board::Board_PortRead, this, std::placeholders::_1));

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

void Board::Board_PortWritten(const PortEventArgs& portEvent) {
	auto port = portEvent.getPort();
	auto value = m_ports.readOutputPort(port);
	std::cout
		<< "Port written: Port: "
		<< Disassembler::hex(port) << ", value: " << Disassembler::hex(value)
		<< '\t'
		<< uint8_t(value + 'A') << std::endl;
}

void Board::Board_PortRead(const PortEventArgs& portEvent) {
	auto port = portEvent.getPort();
	std::cout << "Port read: Port: " << (int)port << std::endl;
}

void Board::Cpu_ExecutingInstruction_Debug(const CpuEventArgs& cpuEvent) {

	const auto& cpu = cpuEvent.getCpu();
	const auto pc = cpu.getProgramCounter();
	const auto instruction = cpu.getMemory().get(pc);
	if ((instruction == 0x3c) || (instruction == 0xc2))
		return;

	m_profiler.addAddress(pc);
	m_profiler.addInstruction(m_memory.get(pc));

	std::cerr
		<< Disassembler::state(cpuEvent.getCpu())
		<< "\t"
		<< Disassembler::disassemble(cpuEvent.getCpu())
		<< '\n';
}