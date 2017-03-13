#include "stdafx.h"
#include "Intel8080.h"

#include "Memory.h"
#include "Disassembler.h"

Intel8080::Intel8080(Memory& memory, InputOutput& ports)
:	m_memory(memory),
	m_ports(ports),
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

Intel8080::Instruction Intel8080::INS(instruction_t method, AddressingMode mode, std::string disassembly, uint64_t cycles) {
	Intel8080::Instruction returnValue;
	returnValue.vector = method;
	returnValue.mode = mode;
	returnValue.disassembly = disassembly;
	returnValue.count = cycles;
	return returnValue;
}

Intel8080::Instruction Intel8080::UNKNOWN() {
	Intel8080::Instruction returnValue;
	returnValue.vector = std::bind(&Intel8080::___, this);
	returnValue.mode = Unknown;
	returnValue.disassembly = "";
	returnValue.count = 0;
	return returnValue;
}

#define BIND(method)	std::bind(&Intel8080:: method, this)

void Intel8080::installInstructions() {
	instructions = {
		////	0									1											2											3										4										5												6											7											8					9											A											B										C											D											E											F
		/* 0 */	INS(BIND(nop), Implied, "NOP", 4),	INS(BIND(lxi_b), Absolute, "LXI B,", 10),	UNKNOWN(),									UNKNOWN(),								UNKNOWN(),								INS(BIND(dcr_b), Implied, "DCR B", 5),			INS(BIND(mvi_b), Immediate, "MVI B,", 7),	UNKNOWN(),									UNKNOWN(),			INS(BIND(dad_b), Implied, "DAD B", 10),		UNKNOWN(),									UNKNOWN(),								UNKNOWN(),									UNKNOWN(),									INS(BIND(mvi_c), Immediate, "MVI C,", 10),	INS(BIND(rrc), Implied, "RRC", 4),			//	0
		/* 1 */	UNKNOWN(),							INS(BIND(lxi_d), Absolute, "LXI D,", 10),	UNKNOWN(),									INS(BIND(inx_d), Implied, "INX D", 5),	INS(BIND(inr_d), Implied, "INR D", 5),	UNKNOWN(),										UNKNOWN(),									UNKNOWN(),									UNKNOWN(),			INS(BIND(dad_d), Implied, "DAD D", 10),		INS(BIND(ldax_d), Implied, "LDAX D", 7),	UNKNOWN(),								UNKNOWN(),									UNKNOWN(),									UNKNOWN(),									UNKNOWN(),									//	1
		/* 2 */	UNKNOWN(),							INS(BIND(lxi_h), Absolute, "LXI H,", 10),	UNKNOWN(),									INS(BIND(inx_h), Implied, "INX H", 5),	INS(BIND(inr_h), Implied, "INR H", 5),	UNKNOWN(),										INS(BIND(mvi_h), Immediate, "MVI H,",7),	UNKNOWN(),									UNKNOWN(),			INS(BIND(dad_h), Implied, "DAD H", 10),		UNKNOWN(),									UNKNOWN(),								UNKNOWN(),									UNKNOWN(),									UNKNOWN(),									UNKNOWN(),									//	2
		/* 3 */	UNKNOWN(),							INS(BIND(lxi_sp), Absolute, "LXI SP,", 10),	INS(BIND(sta), Absolute, "STA ", 13),		UNKNOWN(),								UNKNOWN(),								UNKNOWN(),										INS(BIND(mvi_m), Immediate, "MVI M,", 10),	UNKNOWN(),									UNKNOWN(),			INS(BIND(dad_sp), Implied, "DAD SP", 10),	INS(BIND(lda), Absolute, "LDA ", 13),		UNKNOWN(),								UNKNOWN(),									UNKNOWN(),									UNKNOWN(),									UNKNOWN(),									//	3
		/* 4 */	UNKNOWN(),							UNKNOWN(),									UNKNOWN(),									UNKNOWN(),								UNKNOWN(),								UNKNOWN(),										UNKNOWN(),									UNKNOWN(),									UNKNOWN(),			UNKNOWN(),									UNKNOWN(),									UNKNOWN(),								UNKNOWN(),									UNKNOWN(),									UNKNOWN(),									UNKNOWN(),									//	4
		/* 5 */	UNKNOWN(),							UNKNOWN(),									UNKNOWN(),									UNKNOWN(),								UNKNOWN(),								UNKNOWN(),										UNKNOWN(),									UNKNOWN(),									UNKNOWN(),			UNKNOWN(),									UNKNOWN(),									UNKNOWN(),								INS(BIND(mov_e_h), Implied, "MOV E,H", 5),	UNKNOWN(),									UNKNOWN(),									INS(BIND(mov_e_a), Implied, "MOV E,A", 5),	//	5
		/* 6 */	UNKNOWN(),							UNKNOWN(),									UNKNOWN(),									UNKNOWN(),								UNKNOWN(),								UNKNOWN(),										UNKNOWN(),									UNKNOWN(),									UNKNOWN(),			UNKNOWN(),									UNKNOWN(),									UNKNOWN(),								UNKNOWN(),									UNKNOWN(),									UNKNOWN(),									INS(BIND(mov_l_a), Implied, "MOV L,A", 5),	//	6
		/* 7 */	UNKNOWN(),							UNKNOWN(),									INS(BIND(mov_m_d), Implied, "MOV M,D", 7),	UNKNOWN(),								UNKNOWN(),								UNKNOWN(),										UNKNOWN(),									INS(BIND(mov_m_a), Implied, "MOV M,A", 7),	UNKNOWN(),			UNKNOWN(),									UNKNOWN(),									UNKNOWN(),								INS(BIND(mov_a_h), Implied, "MOV A,H", 5),	UNKNOWN(),									UNKNOWN(),									UNKNOWN(),									//	7
		/* 8 */	UNKNOWN(),							UNKNOWN(),									UNKNOWN(),									UNKNOWN(),								UNKNOWN(),								UNKNOWN(),										UNKNOWN(),									UNKNOWN(),									UNKNOWN(),			UNKNOWN(),									UNKNOWN(),									UNKNOWN(),								UNKNOWN(),									UNKNOWN(),									UNKNOWN(),									UNKNOWN(),									//	8
		/* 9 */	UNKNOWN(),							UNKNOWN(),									UNKNOWN(),									UNKNOWN(),								UNKNOWN(),								UNKNOWN(),										UNKNOWN(),									UNKNOWN(),									UNKNOWN(),			UNKNOWN(),									UNKNOWN(),									UNKNOWN(),								UNKNOWN(),									UNKNOWN(),									UNKNOWN(),									UNKNOWN(),									//	9
		/* A */	UNKNOWN(),							UNKNOWN(),									UNKNOWN(),									UNKNOWN(),								UNKNOWN(),								UNKNOWN(),										UNKNOWN(),									UNKNOWN(),									UNKNOWN(),			UNKNOWN(),									UNKNOWN(),									UNKNOWN(),								UNKNOWN(),									UNKNOWN(),									UNKNOWN(),									INS(BIND(xra_a), Implied, "XRA A", 4),		//	A
		/* B */	UNKNOWN(),							UNKNOWN(),									UNKNOWN(),									UNKNOWN(),								UNKNOWN(),								UNKNOWN(),										UNKNOWN(),									UNKNOWN(),									UNKNOWN(),			UNKNOWN(),									UNKNOWN(),									UNKNOWN(),								UNKNOWN(),									UNKNOWN(),									UNKNOWN(),									UNKNOWN(),									//	B
		/* C */	UNKNOWN(),							INS(BIND(pop_b), Implied, "POP B", 10),		INS(BIND(jnz), Absolute, "JNZ ", 10),		INS(BIND(jmp), Absolute, "JMP ", 10),	UNKNOWN(),								INS(BIND(push_b), Implied, "PUSH B", 11),		INS(BIND(adi), Immediate, "ADI ", 7),		INS(BIND(rst_0), Implied, "RST 0", 11),		UNKNOWN(),			INS(BIND(ret), Implied, "RET", 10),			UNKNOWN(),									UNKNOWN(),								UNKNOWN(),									INS(BIND(call), Absolute, "CALL ", 17),		UNKNOWN(),									INS(BIND(rst_1), Implied, "RST 1", 11),		//	C
		/* D */	UNKNOWN(),							INS(BIND(pop_d), Implied, "POP D", 10),		UNKNOWN(),									INS(BIND(out), Immediate, "OUT ", 10),	UNKNOWN(),								INS(BIND(push_d), Implied, "PUSH D", 11),		UNKNOWN(),									INS(BIND(rst_2), Implied, "RST 2", 11),		UNKNOWN(),			UNKNOWN(),									UNKNOWN(),									UNKNOWN(),								UNKNOWN(),									UNKNOWN(),									UNKNOWN(),									INS(BIND(rst_3), Implied, "RST 3", 11),		//	D
		/* E */	UNKNOWN(),							INS(BIND(pop_h), Implied, "POP H", 10),		UNKNOWN(),									UNKNOWN(),								UNKNOWN(),								INS(BIND(push_h), Implied, "PUSH H", 11),		INS(BIND(ani), Immediate, "ANI ", 7),		INS(BIND(rst_4), Implied, "RST 4", 11),		UNKNOWN(),			UNKNOWN(),									UNKNOWN(),									INS(BIND(xchg), Implied, "XCHG", 4),	UNKNOWN(),									UNKNOWN(),									UNKNOWN(),									INS(BIND(rst_5), Implied, "RST 5", 11),		//	E
		/* F */	UNKNOWN(),							INS(BIND(pop_psw), Implied, "POP PSW", 10),	UNKNOWN(),									UNKNOWN(),								UNKNOWN(),								INS(BIND(push_psw), Implied, "PUSH PSW", 11),	UNKNOWN(),									INS(BIND(rst_6), Implied, "RST 6", 11),		UNKNOWN(),			UNKNOWN(),									UNKNOWN(),									UNKNOWN(),								UNKNOWN(),									UNKNOWN(),									INS(BIND(cpi), Immediate, "CPI ", 7),		INS(BIND(rst_7), Implied, "RST 7", 11),		//	F
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

	ExecutingInstruction.fire(CpuEventArgs(*this));

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

void Intel8080::lxi_b() {
	c = fetchByte();
	b = fetchByte();
}

void Intel8080::lxi_d() {
	e = fetchByte();
	d = fetchByte();
}

void Intel8080::lxi_h() {
	l = fetchByte();
	h = fetchByte();
}

void Intel8080::inr_d() {
	adjustSZP(++d);
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

void Intel8080::push_b() {
	auto pair = Memory::makeWord(c, b);
	pushWord(pair);
}

void Intel8080::push_d() {
	auto pair = Memory::makeWord(e, d);
	pushWord(pair);
}

void Intel8080::push_h() {
	auto pair = Memory::makeWord(l, h);
	pushWord(pair);
}

void Intel8080::push_psw() {
	auto pair = Memory::makeWord(f, a);
	pushWord(pair);
}

void Intel8080::pop_b() {
	auto bc = popWord();
	b = Memory::highByte(bc);
	c = Memory::lowByte(bc);
}

void Intel8080::pop_d() {
	auto de = popWord();
	d = Memory::highByte(de);
	e = Memory::lowByte(de);
}

void Intel8080::pop_h() {
	auto hl = popWord();
	h = Memory::highByte(hl);
	l = Memory::lowByte(hl);
}

void Intel8080::pop_psw() {
	auto af = popWord();
	a = Memory::highByte(af);
	f = Memory::lowByte(af);
	resetUnusedFlags();
}

void Intel8080::call() {
	auto destination = m_memory.getWord(pc);
	callAddress(destination);
}

void Intel8080::rst_0() {
	restart(0);
}

void Intel8080::rst_1() {
	restart(1);
}

void Intel8080::rst_2() {
	restart(2);
}

void Intel8080::rst_3() {
	restart(3);
}

void Intel8080::rst_4() {
	restart(4);
}

void Intel8080::rst_5() {
	restart(5);
}

void Intel8080::rst_6() {
	restart(6);
}

void Intel8080::rst_7() {
	restart(7);
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

void Intel8080::mov_m_d() {
	auto hl = Memory::makeWord(l, h);
	m_memory.set(hl, d);
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
	if (!(f & F_Z))
		pc = destination;
}

void Intel8080::lda() {
	auto source = fetchWord();
	a = m_memory.get(source);
}

void Intel8080::sta() {
	auto destination = fetchWord();
	m_memory.set(destination, a);
}

void Intel8080::ani() {
	resetCarry();
	adjustSZP(a &= fetchByte());
}

void Intel8080::xra_a() {
	resetCarry();
	adjustSZP(a ^= fetchByte());
}

void Intel8080::cpi() {
	compare(fetchByte());
}

void Intel8080::dad_b() {
	auto bc = Memory::makeWord(c, b);
	auto hl = Memory::makeWord(l, h);
	uint32_t sum = bc + hl;
	sum & 0x10000 ? setCarry() : resetCarry();
	h = Memory::highByte(sum);
	l = Memory::lowByte(sum);
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

void Intel8080::dad_sp() {
	auto hl = Memory::makeWord(l, h);
	uint32_t sum = sp + hl;
	sum & 0x10000 ? setCarry() : resetCarry();
	h = Memory::highByte(sum);
	l = Memory::lowByte(sum);
}

void Intel8080::xchg() {
	std::swap(d, h);
	std::swap(e, l);
}

void Intel8080::out() {
	auto port = fetchByte();
	m_ports.write(port, a);
}

void Intel8080::rrc() {
	auto carry = a & 1;
	a >>= 1;
	a &= carry << 7;
	carry ? setCarry() : resetCarry();
}

void Intel8080::adi() {
	auto immediate = fetchByte();
	uint16_t sum = a + immediate;
	a = Memory::lowByte(sum);
	sum > 0xff ? setCarry() : resetCarry();
	adjustSZP(a);
}
