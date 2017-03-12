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
	f(0),
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
		////	0					1						2					3						4						5						6						7						8					9						A						B					C						D						E						F
		/* 0 */	INS(BIND(nop), 4),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(dcr_b), 5),	INS(BIND(mvi_b), 7),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(mvi_c), 10),	INS(BIND(___), 0),		//	0
		/* 1 */	INS(BIND(___), 0),	INS(BIND(lxi_d), 10),	INS(BIND(___), 0),	INS(BIND(inx_d), 5),	INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(dad_d), 10),	INS(BIND(ldax_d), 7),	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		//	1
		/* 2 */	INS(BIND(___), 0),	INS(BIND(lxi_h), 10),	INS(BIND(___), 0),	INS(BIND(inx_h), 5),	INS(BIND(inr_h), 5),	INS(BIND(___), 0),		INS(BIND(mvi_h), 7),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(dad_h), 10),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		//	2
		/* 3 */	INS(BIND(___), 0),	INS(BIND(lxi_sp), 10),	INS(BIND(sta), 13),	INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(mvi_m), 10),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		//	3
		/* 4 */	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		//	4
		/* 5 */	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(mov_e_h), 5),	INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(mov_e_a), 5),	//	5
		/* 6 */	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(mov_l_a), 5),	//	6
		/* 7 */	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(mov_m_a), 7),	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(mov_a_h), 5),	INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		//	7
		/* 8 */	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		//	8
		/* 9 */	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		//	9
		/* A */	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		//	A
		/* B */	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		//	B
		/* C */	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(jnz), 10),	INS(BIND(jmp), 10),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(ret), 10),		INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(call), 17),	INS(BIND(___), 0),		INS(BIND(___), 0),		//	C
		/* D */	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(out), 10),		INS(BIND(___), 0),		INS(BIND(push_d), 11),	INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		//	D
		/* E */	INS(BIND(___), 0),	INS(BIND(pop_h), 10),	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(push_h), 11),	INS(BIND(ani), 7),		INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(xchg), 4),	INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		//	E
		/* F */	INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(___), 0),	INS(BIND(___), 0),		INS(BIND(___), 0),		INS(BIND(cpi), 7),		INS(BIND(___), 0),		//	F
	};
}

void Intel8080::reset() {
	pc = 0;
}

void Intel8080::initialise() {
	sp = 0;
	a = f = b = c = d = e = h = l = 0;
	resetFlags();
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
	pc = fetchWord();
}

void Intel8080::lxi_sp() {
	sp = fetchWord();
}

void Intel8080::lxi_d() {
	e = fetchByte();
	d = fetchByte();
}

void Intel8080::lxi_h() {
	l = fetchByte();
	h = fetchByte();
}

void Intel8080::inr_h() {
	adjustSZP(++h);
}

void Intel8080::mvi_b() {
	b = fetchByte();
}

void Intel8080::mvi_c() {
	c = fetchByte();
}

void Intel8080::mvi_h() {
	h = fetchByte();
}

void Intel8080::mvi_m() {
	auto hl = Memory::makeWord(l, h);
	auto data = fetchByte();
	m_memory.set(hl, data);
}

void Intel8080::push_d() {
	auto pair = Memory::makeWord(e, d);
	pushWord(pair);
}

void Intel8080::push_h() {
	auto pair = Memory::makeWord(l, h);
	pushWord(pair);
}

void Intel8080::pop_h() {
	auto hl = popWord();
	h = Memory::highByte(hl);
	l = Memory::lowByte(hl);
}

void Intel8080::call() {
	auto destination = m_memory.getWord(pc);
	pushWord(pc - 1);
	pc = destination;
}

void Intel8080::ret() {
	pc = popWord() + 1;
}

void Intel8080::ldax_d() {
	auto de = Memory::makeWord(e, d);
	a = m_memory.get(de);
}

void Intel8080::mov_m_a() {
	auto hl = Memory::makeWord(l, h);
	m_memory.set(hl, a);
}

void Intel8080::mov_a_h() {
	a = h;
}

void Intel8080::mov_e_a() {
	e = a;
}

void Intel8080::mov_e_h() {
	e = h;
}

void Intel8080::mov_l_a() {
	l = a;
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
	adjustSZP(--b);
}

void Intel8080::jnz() {
	auto destination  = fetchWord();
	if (f & F_Z)
		pc = destination;
}

void Intel8080::sta() {
	auto destination = fetchWord();
	m_memory.set(destination, a);
}

void Intel8080::ani() {
	resetCarry();
	adjustSZP(a &= fetchByte());
}

void Intel8080::cpi() {
	compare(fetchByte());
}

void Intel8080::dad_d() {
	auto de = Memory::makeWord(e, d);
	auto hl = Memory::makeWord(l, h);
	uint32_t sum = de + hl;
	sum & 0x10000 ? setCarry() : resetCarry();
	h = Memory::highByte(sum);
	l = Memory::lowByte(sum);
}

void Intel8080::dad_h() {
	auto hl = Memory::makeWord(l, h);
	uint32_t doubled = hl << 1;
	doubled & 0x10000 ? setCarry() : resetCarry();
	h = Memory::highByte(hl);
	l = Memory::lowByte(hl);
}

void Intel8080::xchg() {
	std::swap(d, h);
	std::swap(e, l);
}

void Intel8080::out() {
	auto port = fetchByte();
	ports[port] = a;
}
