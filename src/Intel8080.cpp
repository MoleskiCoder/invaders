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
		////	0					1						2					3						4						5						6						7						8					9					A						B					C					D					E					F
		/* 0 */	INS(BIND(nop), 4),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(dcr_b), 5),	INS(BIND(mvi_b), 7),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),
		/* 1 */	INS(BIND(___), 0),	INS(BIND(lxi_d), 10),	INS(BIND(___), 0),	INS(BIND(inx_d), 5),	INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(ldax_d), 7),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),
		/* 2 */	INS(BIND(___), 0),	INS(BIND(lxi_h), 10),	INS(BIND(___), 0),	INS(BIND(inx_h), 5),	INS(BIND(inr_h), 5),	INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),
		/* 3 */	INS(BIND(___), 0),	INS(BIND(lxi_sp), 10),	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),
		/* 4 */	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),
		/* 5 */	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),
		/* 6 */	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),
		/* 7 */	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(mov_m_a), 7),	INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),
		/* 8 */	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),
		/* 9 */	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),
		/* A */	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),
		/* B */	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),
		/* C */	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(jmp), 10),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(call), 17),	INS(BIND(___), 0),	INS(BIND(___), 0),
		/* D */	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),
		/* E */	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),
		/* F */	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),
	};
}

void Intel8080::reset() {
	pc = 0;
}

void Intel8080::initialise() {
	sp = 0;
	a = b = c = d = e = h = l = 0;
	reset();
}

void Intel8080::step() {
	auto opcode = m_memory.get(pc++);
	auto instruction = instructions[opcode];
	instruction.vector();
	cycles += instruction.count;
}

//

void Intel8080::pushByte(uint8_t value) {
	m_memory.set(sp--, value);
}

uint8_t Intel8080::popByte() {
	return m_memory.get(++sp);
}

void Intel8080::pushWord(uint16_t value) {
	pushByte(Memory::highByte(value));
	pushByte(Memory::lowByte(value));
}

uint16_t Intel8080::popWord() {
	auto low = popByte();
	auto high = popByte();
	return Memory::makeWord(low, high);
}

//

void Intel8080::___() {
	auto opcode = m_memory.get(pc - 1);
	auto message = Disassembler::invalid(opcode);
	throw new std::domain_error(message);
}

void Intel8080::nop() {
}

void Intel8080::jmp() {
	pc = m_memory.getWord(pc);
}

void Intel8080::lxi_sp() {
	sp = m_memory.getWord(pc);
	pc += 2;
}

void Intel8080::lxi_d() {
	e = m_memory.get(pc);
	d = m_memory.get(pc + 1);
	pc += 2;
}

void Intel8080::lxi_h() {
	l = m_memory.get(pc);
	h = m_memory.get(pc + 1);
	pc += 2;
}

void Intel8080::inr_h() {
	++h;
}

void Intel8080::mvi_b() {
	b = m_memory.get(pc++);
}

void Intel8080::call() {
	auto destination = m_memory.getWord(pc);
	pushWord(pc - 1);
	pc = destination;
}

void Intel8080::ldax_d() {
	auto de = Memory::makeWord(e, d);
	a = m_memory.get(de);
}

void Intel8080::mov_m_a() {
	auto hl = Memory::makeWord(l, h);
	m_memory.set(hl, a);
}

void Intel8080::inx_d() {
	auto de = Memory::makeWord(e, d);
	e = Memory::lowByte(++de);
	d = Memory::lowByte(de);
}

void Intel8080::inx_h() {
	auto hl = Memory::makeWord(l, h);
	l = Memory::lowByte(++hl);
	h = Memory::lowByte(hl);
}

void Intel8080::dcr_b() {
	--b;
}
