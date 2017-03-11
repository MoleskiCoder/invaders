#include "stdafx.h"
#include "Intel8080.h"

#include "Memory.h"
#include "Disassembler.h"

Intel8080::Intel8080(Memory& memory)
:	m_memory(memory),
	cycles(0),
	pc(0),
	sp(0),
	a(0),
	b(0),
	c(0),
	d(0),
	e(0),
	h(0),
	l(0) {
	installInstructions();
}

Intel8080::Instruction Intel8080::INS(instruction_t method, uint64_t cycles) {
	Intel8080::Instruction returnValue;
	returnValue.vector = method;
	returnValue.count = cycles;
	return returnValue;
}

#define BIND(method)	std::bind(&Intel8080:: method, this)

void Intel8080::installInstructions() {
	instructions = {
		////	0					1						2					3					4						5					6					7					8					9					A					B					C					D					E					F
		/* 0 */	INS(BIND(nop), 4),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),
		/* 1 */	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),
		/* 2 */	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(inr_h), 5),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),
		/* 3 */	INS(BIND(___), 0),	INS(BIND(lxi_sp), 10),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),
		/* 4 */	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),
		/* 5 */	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),
		/* 6 */	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),
		/* 7 */	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),
		/* 8 */	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),
		/* 9 */	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),
		/* A */	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),
		/* B */	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),
		/* C */	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(jmp), 10),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),
		/* D */	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),
		/* E */	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),
		/* F */	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),
	};
}

void Intel8080::reset() {
	pc = 0;
}

void Intel8080::initialise() {
	pc = sp = 0;
	a = b = c = d = e = h = l = 0;
	reset();
}

void Intel8080::step() {
	auto opcode = m_memory.get(pc++);
	auto instruction = instructions[opcode];
	instruction.vector();
	cycles += instruction.count;
}

void Intel8080::___() {
	auto opcode = m_memory.get(pc - 1);
	auto message = Disassembler::invalid(opcode);
	throw new std::domain_error(message);
}

void Intel8080::nop() {
}

void Intel8080::jmp() {
	auto destination = m_memory.getWord(pc);
	pc = destination;
}

void Intel8080::lxi_sp() {
	auto word = m_memory.getWord(pc);
	sp = word;
}

void Intel8080::inr_h() {
	++h;
}
