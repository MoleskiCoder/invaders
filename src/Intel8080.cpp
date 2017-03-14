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
	l(0),
	m_interrupt(false),
	m_halted(false) {
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
		////	0											1											2											3											4											5												6											7											8											9											A											B											C											D											E											F
		/* 0 */	INS(BIND(nop), Implied, "NOP", 4),			INS(BIND(lxi_b), Absolute, "LXI B,", 10),	UNKNOWN(),									UNKNOWN(),									UNKNOWN(),									INS(BIND(dcr_b), Implied, "DCR B", 5),			INS(BIND(mvi_b), Immediate, "MVI B,", 7),	UNKNOWN(),									UNKNOWN(),									INS(BIND(dad_b), Implied, "DAD B", 10),		INS(BIND(ldax_b), Implied, "LDAX B", 7),	UNKNOWN(),									UNKNOWN(),									UNKNOWN(),									INS(BIND(mvi_c), Immediate, "MVI C,", 10),	INS(BIND(rrc), Implied, "RRC", 4),			//	0
		/* 1 */	UNKNOWN(),									INS(BIND(lxi_d), Absolute, "LXI D,", 10),	UNKNOWN(),									INS(BIND(inx_d), Implied, "INX D", 5),		INS(BIND(inr_d), Implied, "INR D", 5),		UNKNOWN(),										UNKNOWN(),									UNKNOWN(),									UNKNOWN(),									INS(BIND(dad_d), Implied, "DAD D", 10),		INS(BIND(ldax_d), Implied, "LDAX D", 7),	UNKNOWN(),									UNKNOWN(),									UNKNOWN(),									UNKNOWN(),									UNKNOWN(),									//	1
		/* 2 */	UNKNOWN(),									INS(BIND(lxi_h), Absolute, "LXI H,", 10),	UNKNOWN(),									INS(BIND(inx_h), Implied, "INX H", 5),		INS(BIND(inr_h), Implied, "INR H", 5),		UNKNOWN(),										INS(BIND(mvi_h), Immediate, "MVI H,",7),	UNKNOWN(),									UNKNOWN(),									INS(BIND(dad_h), Implied, "DAD H", 10),		UNKNOWN(),									UNKNOWN(),									UNKNOWN(),									UNKNOWN(),									UNKNOWN(),									UNKNOWN(),									//	2
		/* 3 */	UNKNOWN(),									INS(BIND(lxi_sp), Absolute, "LXI SP,", 10),	INS(BIND(sta), Absolute, "STA ", 13),		UNKNOWN(),									UNKNOWN(),									UNKNOWN(),										INS(BIND(mvi_m), Immediate, "MVI M,", 10),	UNKNOWN(),									UNKNOWN(),									INS(BIND(dad_sp), Implied, "DAD SP", 10),	INS(BIND(lda), Absolute, "LDA ", 13),		UNKNOWN(),									UNKNOWN(),									UNKNOWN(),									UNKNOWN(),									UNKNOWN(),									//	3
		/* 4 */	INS(BIND(mov_b_b), Implied, "MOV B,B", 5),	INS(BIND(mov_b_c), Implied, "MOV B,C", 5),	INS(BIND(mov_b_d), Implied, "MOV B,D", 5),	INS(BIND(mov_b_e), Implied, "MOV B,E", 5),	INS(BIND(mov_b_h), Implied, "MOV B,H", 5),	INS(BIND(mov_b_l), Implied, "MOV B,L", 5),		INS(BIND(mov_b_m), Implied, "MOV B,M", 7),	INS(BIND(mov_b_a), Implied, "MOV B,A", 5),	INS(BIND(mov_c_b), Implied, "MOV C,B", 5),	INS(BIND(mov_c_c), Implied, "MOV C,C", 5),	INS(BIND(mov_c_d), Implied, "MOV C,D", 5),	INS(BIND(mov_c_e), Implied, "MOV C,E", 5),	INS(BIND(mov_c_h), Implied, "MOV C,H", 5),	INS(BIND(mov_c_l), Implied, "MOV C,L", 5),	INS(BIND(mov_c_m), Implied, "MOV C,M", 7),	INS(BIND(mov_c_a), Implied, "MOV C,A", 5),	//	4
		/* 5 */	INS(BIND(mov_d_b), Implied, "MOV D,B", 5),	INS(BIND(mov_d_c), Implied, "MOV D,C", 5),	INS(BIND(mov_d_d), Implied, "MOV D,D", 5),	INS(BIND(mov_d_e), Implied, "MOV D,E", 5),	INS(BIND(mov_d_h), Implied, "MOV D,H", 5),	INS(BIND(mov_d_l), Implied, "MOV D,L", 5),		INS(BIND(mov_d_m), Implied, "MOV D,M", 7),	INS(BIND(mov_d_a), Implied, "MOV D,A", 5),	INS(BIND(mov_e_b), Implied, "MOV E,B", 5),	INS(BIND(mov_e_c), Implied, "MOV E,C", 5),	INS(BIND(mov_e_d), Implied, "MOV E,D", 5),	INS(BIND(mov_e_e), Implied, "MOV E,E", 5),	INS(BIND(mov_e_h), Implied, "MOV E,H", 5),	INS(BIND(mov_e_l), Implied, "MOV E,L", 5),	INS(BIND(mov_e_m), Implied, "MOV E,M", 7),	INS(BIND(mov_e_a), Implied, "MOV E,A", 5),	//	5
		/* 6 */	INS(BIND(mov_h_b), Implied, "MOV H,B", 5),	INS(BIND(mov_h_c), Implied, "MOV H,C", 5),	INS(BIND(mov_h_d), Implied, "MOV H,D", 5),	INS(BIND(mov_h_e), Implied, "MOV H,E", 5),	INS(BIND(mov_h_h), Implied, "MOV H,H", 5),	INS(BIND(mov_h_l), Implied, "MOV H,L", 5),		INS(BIND(mov_h_m), Implied, "MOV H,M", 7),	INS(BIND(mov_h_a), Implied, "MOV H,A", 5),	INS(BIND(mov_l_b), Implied, "MOV L,B", 5),	INS(BIND(mov_l_c), Implied, "MOV L,C", 5),	INS(BIND(mov_l_d), Implied, "MOV L,D", 5),	INS(BIND(mov_l_e), Implied, "MOV L,E", 5),	INS(BIND(mov_l_h), Implied, "MOV L,H", 5),	INS(BIND(mov_l_l), Implied, "MOV L,L", 5),	INS(BIND(mov_l_m), Implied, "MOV L,M", 7),	INS(BIND(mov_l_a), Implied, "MOV L,A", 5),	//	6
		/* 7 */	INS(BIND(mov_m_b), Implied, "MOV M,B", 7),	INS(BIND(mov_m_c), Implied, "MOV M,C", 7),	INS(BIND(mov_m_d), Implied, "MOV M,D", 7),	INS(BIND(mov_m_e), Implied, "MOV M,E", 7),	INS(BIND(mov_m_h), Implied, "MOV M,H", 7),	INS(BIND(mov_m_l), Implied, "MOV M,L", 7),		INS(BIND(hlt), Implied, "HLT", 7),			INS(BIND(mov_m_a), Implied, "MOV M,A", 7),	INS(BIND(mov_a_b), Implied, "MOV A,B", 5),	INS(BIND(mov_a_c), Implied, "MOV A,C", 5),	INS(BIND(mov_a_d), Implied, "MOV A,D", 5),	INS(BIND(mov_a_e), Implied, "MOV A,E", 5),	INS(BIND(mov_a_h), Implied, "MOV A,H", 5),	INS(BIND(mov_a_l), Implied, "MOV A,L", 5),	INS(BIND(mov_a_m), Implied, "MOV A,M", 7),	INS(BIND(mov_a_a), Implied, "MOV A,A", 5),	//	7
		/* 8 */	INS(BIND(add_b), Implied, "ADD B", 4),		UNKNOWN(),									UNKNOWN(),									UNKNOWN(),									UNKNOWN(),									UNKNOWN(),										UNKNOWN(),									UNKNOWN(),									UNKNOWN(),									UNKNOWN(),									UNKNOWN(),									UNKNOWN(),									UNKNOWN(),									UNKNOWN(),									UNKNOWN(),									UNKNOWN(),									//	8
		/* 9 */	INS(BIND(sub_b), Implied, "SUB B", 4),		UNKNOWN(),									UNKNOWN(),									UNKNOWN(),									UNKNOWN(),									UNKNOWN(),										UNKNOWN(),									UNKNOWN(),									UNKNOWN(),									UNKNOWN(),									UNKNOWN(),									UNKNOWN(),									UNKNOWN(),									UNKNOWN(),									UNKNOWN(),									UNKNOWN(),									//	9
		/* A */	INS(BIND(ana_b), Implied, "ANA B", 4),		UNKNOWN(),									UNKNOWN(),									UNKNOWN(),									UNKNOWN(),									UNKNOWN(),										UNKNOWN(),									INS(BIND(ana_a), Implied, "ANA A", 4),		UNKNOWN(),									UNKNOWN(),									UNKNOWN(),									UNKNOWN(),									UNKNOWN(),									UNKNOWN(),									UNKNOWN(),									INS(BIND(xra_a), Implied, "XRA A", 4),		//	A
		/* B */	INS(BIND(ora_b), Implied, "ORA B", 4),		UNKNOWN(),									UNKNOWN(),									UNKNOWN(),									UNKNOWN(),									UNKNOWN(),										UNKNOWN(),									UNKNOWN(),									UNKNOWN(),									UNKNOWN(),									UNKNOWN(),									UNKNOWN(),									UNKNOWN(),									UNKNOWN(),									UNKNOWN(),									INS(BIND(cmp_a), Implied, "CMP A", 4),		//	B
		/* C */	INS(BIND(rnz), Implied, "RNZ", 5),			INS(BIND(pop_b), Implied, "POP B", 10),		INS(BIND(jnz), Absolute, "JNZ ", 10),		INS(BIND(jmp), Absolute, "JMP ", 10),		UNKNOWN(),									INS(BIND(push_b), Implied, "PUSH B", 11),		INS(BIND(adi), Immediate, "ADI ", 7),		INS(BIND(rst_0), Implied, "RST 0", 11),		UNKNOWN(),									INS(BIND(ret), Implied, "RET", 10),			UNKNOWN(),									UNKNOWN(),									UNKNOWN(),									INS(BIND(call), Absolute, "CALL ", 17),		UNKNOWN(),									INS(BIND(rst_1), Implied, "RST 1", 11),		//	C
		/* D */	INS(BIND(rnc), Implied, "RNC", 5),			INS(BIND(pop_d), Implied, "POP D", 10),		UNKNOWN(),									INS(BIND(out), Immediate, "OUT ", 10),		UNKNOWN(),									INS(BIND(push_d), Implied, "PUSH D", 11),		UNKNOWN(),									INS(BIND(rst_2), Implied, "RST 2", 11),		UNKNOWN(),									UNKNOWN(),									INS(BIND(jc), Absolute, "JC ", 10),			UNKNOWN(),									UNKNOWN(),									UNKNOWN(),									UNKNOWN(),									INS(BIND(rst_3), Implied, "RST 3", 11),		//	D
		/* E */	INS(BIND(rpo), Implied, "RPO", 5),			INS(BIND(pop_h), Implied, "POP H", 10),		UNKNOWN(),									UNKNOWN(),									UNKNOWN(),									INS(BIND(push_h), Implied, "PUSH H", 11),		INS(BIND(ani), Immediate, "ANI ", 7),		INS(BIND(rst_4), Implied, "RST 4", 11),		UNKNOWN(),									INS(BIND(pchl), Implied, "PCHL", 5),		UNKNOWN(),									INS(BIND(xchg), Implied, "XCHG", 4),		UNKNOWN(),									UNKNOWN(),									UNKNOWN(),									INS(BIND(rst_5), Implied, "RST 5", 11),		//	E
		/* F */	INS(BIND(rp), Implied, "RP", 5),			INS(BIND(pop_psw), Implied, "POP PSW", 10),	UNKNOWN(),									INS(BIND(di), Implied, "DI ", 4),			UNKNOWN(),									INS(BIND(push_psw), Implied, "PUSH PSW", 11),	UNKNOWN(),									INS(BIND(rst_6), Implied, "RST 6", 11),		UNKNOWN(),									UNKNOWN(),									UNKNOWN(),									INS(BIND(ei), Implied, "EI", 4),			UNKNOWN(),									UNKNOWN(),									INS(BIND(cpi), Immediate, "CPI ", 7),		INS(BIND(rst_7), Implied, "RST 7", 11),		//	F
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
