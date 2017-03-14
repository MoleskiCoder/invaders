#include "stdafx.h"
#include "Disassembler.h"

#include <sstream>
#include <iomanip>
#include <bitset>

#include "Memory.h"
#include "Intel8080.h"

Disassembler::Disassembler() {
}

std::string Disassembler::state(const Intel8080& cpu) {

	auto pc = cpu.getProgramCounter();
	auto sp = cpu.getStackPointer();

	auto a = cpu.getA();
	auto f = cpu.getF();

	auto b = cpu.getB();
	auto c = cpu.getC();

	auto d = cpu.getD();
	auto e = cpu.getE();

	auto h = cpu.getH();
	auto l = cpu.getL();

	std::ostringstream output;

	output
		<< "PC=" << hex(pc)
		<< " "
		<< "SP=" << hex(sp)
		<< " "
		<< "F=" << flag(f)
		<< " "
		<< "A=" << hex(a)
		<< " "
		<< "H=" << hex(h)
		<< " "
		<< "L=" << hex(l);

	return output.str();
}

std::string Disassembler::disassemble(const Intel8080& cpu) {

	const auto& memory = cpu.getMemory();
	auto pc = cpu.getProgramCounter();
	auto opcode = memory.get(pc);
	const auto& instruction = cpu.getInstructions()[opcode];

	std::ostringstream output;

	// hex opcode
	output << hex(opcode);

	// hex raw operand
	switch (instruction.mode) {
	case Intel8080::Immediate:
		output << hex(memory.get(pc + 1));
		break;
	case Intel8080::Absolute:
		output << hex(memory.get(pc + 1));
		output << hex(memory.get(pc + 2));
		break;
	default:
		break;
	}
	output << "\t";

	// base disassembly
	output << instruction.disassembly;

	// disassembly operand
	switch (instruction.mode) {
	case Intel8080::Immediate:
		output << hex(memory.get(pc + 1));
		break;
	case Intel8080::Absolute:
		output << hex(memory.getWord(pc + 1));
		break;
	default:
		break;
	}

	return output.str();
}

std::string Disassembler::flag(uint8_t value) {
	std::ostringstream output;
	output
		<< (value & Intel8080::F_S ? "S" : "-")
		<< (value & Intel8080::F_Z ? "Z" : "-")
		<< "0"
		<< (value & Intel8080::F_AC ? "A" : "-")
		<< "0"
		<< (value & Intel8080::F_P ? "P" : "-")
		<< "1"
		<< (value & Intel8080::F_C ? "C" : "-");
	return output.str();
}

std::string Disassembler::hex(uint8_t value) {
	std::ostringstream output;
	output << std::hex << std::setw(2) << std::setfill('0') << (int)value;
	return output.str();
}

std::string Disassembler::hex(uint16_t value) {
	std::ostringstream output;
	output << std::hex << std::setw(4) << std::setfill('0') << (int)value;
	return output.str();
}

std::string Disassembler::binary(uint8_t value) {
	std::ostringstream output;
	output << std::bitset<8>(value);
	return output.str();
}

std::string Disassembler::invalid(uint8_t value) {
	std::ostringstream output;
	output << "Invalid instruction: " << hex(value) << "(" << binary(value) << ")";
	return output.str();
}