#pragma once

#include <cstdint>
#include <array>
#include <functional>
#include <bitset>

#include "Memory.h"
#include "InputOutput.h"
#include "Signal.h"
#include "CpuEventArgs.h"

class Intel8080 {
public:

	typedef std::function<void()> instruction_t;

	enum AddressingMode {
		Unknown,
		Implied,	// zero bytes
		Immediate,	// single byte
		Absolute	// two bytes, little endian
	};

	struct Instruction {
		instruction_t vector = nullptr;
		AddressingMode mode = Unknown;
		std::string disassembly;
		uint64_t count = 0;
	};

	enum {
		F_C = 0x1,
		F_P = 0x4,
		F_AC = 0x10,
		F_Z = 0x40,
		F_S = 0x80
	};

	Intel8080(Memory& memory, InputOutput& ports);

	Signal<CpuEventArgs> ExecutingInstruction;

	const std::array<Instruction, 0x100>& getInstructions() const { return instructions;  }
	const Memory& getMemory() const { return m_memory; }

	uint16_t getProgramCounter() const { return pc; }
	void setProgramCounter(uint16_t value) { pc = value; }

	uint16_t getStackPointer() const { return sp; }

	uint8_t getA() const { return a; }
	uint8_t getF() const { return f; }

	uint8_t getB() const { return b; }
	uint8_t getC() const { return c; }

	uint8_t getD() const { return d; }
	uint8_t getE() const { return e; }

	uint8_t getH() const { return h; }
	uint8_t getL() const { return l; }

	bool isInterruptable() const {
		return m_interrupt;
	}

	void initialise();

	void reset();
	void step();

private:
	std::array<Instruction, 0x100> instructions;

	Memory& m_memory;
	InputOutput& m_ports;

	uint64_t cycles;

	uint16_t pc;
	uint16_t sp;

	uint8_t a;
	uint8_t f;

	uint8_t b;
	uint8_t c;

	uint8_t d;
	uint8_t e;

	uint8_t h;
	uint8_t l;

	bool m_interrupt;
	bool m_halted;

	static uint8_t setFlag(uint8_t status, uint8_t flag) { return status | flag; }
	static uint8_t resetFlag(uint8_t status, uint8_t flag) { return status & ~flag; }

	void setFlag(uint8_t flag) { f = setFlag(f, flag); }
	void resetFlag(uint8_t flag) { f = resetFlag(f, flag); }

	void setSign() { setFlag(F_S); }
	void resetSign() { resetFlag(F_S); }
	void adjustSign(uint8_t value) { value & 0x80 ? setSign() : resetSign(); }

	void setZero() { setFlag(F_Z); }
	void resetZero() { resetFlag(F_Z); }
	void adjustZero(uint8_t value) { value ? resetZero() : setZero(); }

	void setParity() { setFlag(F_P); }
	void resetParity() { resetFlag(F_P); }
	void adjustParity(uint8_t value) { std::bitset<8>(value).count() % 2 ? setParity() : resetParity(); }

	void adjustSZP(uint8_t value) {
		adjustSign(value);
		adjustZero(value);
		adjustParity(value);
	}

	void setCarry() { setFlag(F_C); }
	void resetCarry() { resetFlag(F_C); }

	void setAuxillaryCarry() { setFlag(F_AC); }
	void resetAuxiliaryCarry() { resetFlag(F_AC); }

	void resetUnusedFlags() {
		setFlag(0x2);
		resetFlag(0x8);
		resetFlag(0x20);
	}

	void resetFlags() {
		resetCarry();
		resetParity();
		resetAuxiliaryCarry();
		resetZero();
		resetSign();
		resetUnusedFlags();
	}

	void pushWord(uint16_t value);
	uint16_t popWord();

	uint8_t fetchByte() {
		return m_memory.get(pc++);
	}

	uint16_t fetchWord() {
		auto value = m_memory.getWord(pc);
		pc += 2;
		return value;
	}

	static Instruction INS(instruction_t method, AddressingMode mode, std::string disassembly, uint64_t cycles);
	Instruction UNKNOWN();

	void installInstructions();

	//

	void compare(uint8_t value) {
		uint16_t subtraction = a - value;
		adjustSZP((uint8_t)subtraction);
		subtraction & 0x100 ? setCarry() : resetCarry();
	}

	void callAddress(uint16_t address) {
		pushWord(pc + 2);
		pc = address;
	}

	void restart(uint8_t position) {
		auto address = position << 3;
		callAddress(address);
	}

	void callConditional(int condition) {
		if (condition) {
			call();
		} else {
			pc += 2;
		}
	}

	void returnConditional(int condition) {
		if (condition)
			ret();
	}

	void jmpConditional(int conditional) {
		auto destination = fetchWord();
		if (conditional)
			pc = destination;
	}

	void and(uint8_t value) {
		resetCarry();
		adjustSZP(a &= value);
	}

	void ora(uint8_t value) {
		resetCarry();
		adjustSZP(a |= value);
	}

	void xra(uint8_t value) {
		resetCarry();
		adjustSZP(a ^= value);
	}

	void add(uint8_t value) {
		uint16_t sum = a + value;
		a = Memory::lowByte(sum);
		sum > 0xff ? setCarry() : resetCarry();
		adjustSZP(a);
	}

	void dad(uint16_t value) {
		auto hl = Memory::makeWord(l, h);
		uint32_t sum = hl + value;
		sum & 0x10000 ? setCarry() : resetCarry();
		h = Memory::highByte(sum);
		l = Memory::lowByte(sum);
	}

	void sub(uint8_t value) {
		uint16_t difference = a - value;
		a = Memory::lowByte(difference);
		difference > 0xff ? setCarry() : resetCarry();
		adjustSZP(a);
	}

	void mov_m_r(uint8_t value) {
		auto hl = Memory::makeWord(l, h);
		m_memory.set(hl, value);
	}

	uint8_t mov_r_m() {
		auto hl = Memory::makeWord(l, h);
		return m_memory.get(hl);
	}

	//

	void ___();

	// Move, load, and store

	void mov_a_a() { a = a; }
	void mov_a_b() { a = b; }
	void mov_a_c() { a = c; }
	void mov_a_d() { a = d; }
	void mov_a_e() { a = e; }
	void mov_a_h() { a = h; }
	void mov_a_l() { a = l; }

	void mov_b_a() { b = a; }
	void mov_b_b() { b = b; }
	void mov_b_c() { b = c; }
	void mov_b_d() { b = d; }
	void mov_b_e() { b = e; }
	void mov_b_h() { b = h; }
	void mov_b_l() { b = l; }

	void mov_c_a() { c = a; }
	void mov_c_b() { c = b; }
	void mov_c_c() { c = c; }
	void mov_c_d() { c = d; }
	void mov_c_e() { c = e; }
	void mov_c_h() { c = h; }
	void mov_c_l() { c = l; }

	void mov_d_a() { d = a; }
	void mov_d_b() { d = b; }
	void mov_d_c() { d = c; }
	void mov_d_d() { d = d; }
	void mov_d_e() { d = e; }
	void mov_d_h() { d = h; }
	void mov_d_l() { d = l; }

	void mov_e_a() { e = a; }
	void mov_e_b() { e = b; }
	void mov_e_c() { e = c; }
	void mov_e_d() { e = d; }
	void mov_e_e() { e = e; }
	void mov_e_h() { e = h; }
	void mov_e_l() { e = l; }

	void mov_h_a() { h = a; }
	void mov_h_b() { h = b; }
	void mov_h_c() { h = c; }
	void mov_h_d() { h = d; }
	void mov_h_e() { h = e; }
	void mov_h_h() { h = h; }
	void mov_h_l() { h = l; }

	void mov_l_a() { l = a; }
	void mov_l_b() { l = b; }
	void mov_l_c() { l = c; }
	void mov_l_d() { l = d; }
	void mov_l_e() { l = e; }
	void mov_l_h() { l = h; }
	void mov_l_l() { l = l; }

	void mov_m_a() { mov_m_r(a); }
	void mov_m_b() { mov_m_r(b); }
	void mov_m_c() { mov_m_r(c); }
	void mov_m_d() { mov_m_r(d); }
	void mov_m_e() { mov_m_r(e); }
	void mov_m_h() { mov_m_r(h); }
	void mov_m_l() { mov_m_r(l); }

	void mov_a_m() { a = mov_r_m(); }
	void mov_b_m() { b = mov_r_m(); }
	void mov_c_m() { c = mov_r_m(); }
	void mov_d_m() { d = mov_r_m(); }
	void mov_e_m() { e = mov_r_m(); }
	void mov_h_m() { h = mov_r_m(); }
	void mov_l_m() { l = mov_r_m(); }

	void mvi_a() { a = fetchByte(); }
	void mvi_b() { b = fetchByte(); }
	void mvi_c() { c = fetchByte(); }
	void mvi_h() { h = fetchByte(); }

	void mvi_m() {
		auto data = fetchByte();
		auto hl = Memory::makeWord(l, h);
		m_memory.set(hl, data);
	}

	void lxi_b() {
		c = fetchByte();
		b = fetchByte();
	}

	void lxi_d() {
		e = fetchByte();
		d = fetchByte();
	}

	void lxi_h() {
		l = fetchByte();
		h = fetchByte();
	}

	void ldax_b() {
		auto bc = Memory::makeWord(c, b);
		a = m_memory.get(bc);
	}

	void ldax_d() {
		auto de = Memory::makeWord(e, d);
		a = m_memory.get(de);
	}

	void sta() {
		auto destination = fetchWord();
		m_memory.set(destination, a);
	}

	void lda() {
		auto source = fetchWord();
		a = m_memory.get(source);
	}

	void xchg() {
		std::swap(d, h);
		std::swap(e, l);
	}

	// stack ops

	void push_b() {
		auto pair = Memory::makeWord(c, b);
		pushWord(pair);
	}

	void push_d() {
		auto pair = Memory::makeWord(e, d);
		pushWord(pair);
	}

	void push_h() {
		auto pair = Memory::makeWord(l, h);
		pushWord(pair);
	}

	void push_psw() {
		auto pair = Memory::makeWord(f, a);
		pushWord(pair);
	}

	void pop_b() {
		auto bc = popWord();
		b = Memory::highByte(bc);
		c = Memory::lowByte(bc);
	}

	void pop_d() {
		auto de = popWord();
		d = Memory::highByte(de);
		e = Memory::lowByte(de);
	}

	void pop_h() {
		auto hl = popWord();
		h = Memory::highByte(hl);
		l = Memory::lowByte(hl);
	}

	void pop_psw() {
		auto af = popWord();
		a = Memory::highByte(af);
		f = Memory::lowByte(af);
		resetUnusedFlags();
	}

	void lxi_sp() {
		sp = fetchWord();
	}

	// jump

	void jmp() { jmpConditional(true); }
	
	void jc() { jmpConditional(f & F_C); }
	void jnc() { jmpConditional(!(f & F_C)); }
	
	void jz() { jmpConditional(f & F_Z); }
	void jnz() { jmpConditional(!(f & F_Z)); }
	
	void jpe() { jmpConditional(f & F_P); }
	void jpo() { jmpConditional(!(f & F_P)); }
	
	void jm() { jmpConditional(f & F_S); }
	void jp() { jmpConditional(!(f & F_S));	}
	
	void pchl() {
		pc = Memory::makeWord(l, h);
	}

	// call

	void call() {
		auto destination = m_memory.getWord(pc);
		callAddress(destination);
	}

	void cc() { callConditional(f & F_C); }
	void cnc() { callConditional(!(f & F_C)); }

	void cpe() { callConditional(f & F_P); }
	void cpo() { callConditional(!(f & F_P));  }

	void cz() { callConditional(f & F_Z); }
	void cnz() { callConditional(!(f & F_Z)); }

	void cm() { callConditional(f & F_S); }
	void cp() { callConditional(!(f & F_S)); }

	// return

	void ret() {
		pc = popWord();
	}

	void rc() { returnConditional(f & F_C); }
	void rnc() { returnConditional(!(f & F_C)); }

	void rz() { returnConditional(f & F_Z); }
	void rnz() { returnConditional(!(f & F_Z)); }

	void rpe() { returnConditional(f & F_P); }
	void rpo() { returnConditional(!(f & F_P)); }

	void rm() { returnConditional(f & F_S); }
	void rp() { returnConditional(!(f & F_S)); }

	// restart

	void rst_0() { restart(0); }
	void rst_1() { restart(1); }
	void rst_2() { restart(2); }
	void rst_3() { restart(3); }
	void rst_4() { restart(4); }
	void rst_5() { restart(5); }
	void rst_6() { restart(6); }
	void rst_7() { restart(7); }

	// increment and decrement

	void inr_a() { adjustSZP(++a); }
	void inr_d() { adjustSZP(++d); }
	void inr_h() { adjustSZP(++h); }

	void dcr_b() { adjustSZP(--b); }

	void inx_d() {
		auto de = Memory::makeWord(e, d);
		e = Memory::lowByte(++de);
		d = Memory::lowByte(de);
	}

	void inx_h() {
		auto hl = Memory::makeWord(l, h);
		l = Memory::lowByte(++hl);
		h = Memory::lowByte(hl);
	}

	// add

	void add_b() { add(b); }

	void adi() { add(fetchByte()); }

	void dad_b() {
		auto bc = Memory::makeWord(c, b);
		dad(bc);
	}

	void dad_d() {
		auto de = Memory::makeWord(e, d);
		dad(de);
	}

	void dad_h() {
		auto hl = Memory::makeWord(l, h);
		dad(hl);
	}

	void dad_sp() {
		dad(sp);
	}

	// subtract

	void sub_b() {
		sub(b);
	}

	// logical

	void ana_a() { and(a); }
	void ana_b() { and(b); }

	void xra_a() { xra(a); }

	void ora_b() { ora(b); }

	void cmp_a() { compare(a); }

	void ani() { and(fetchByte()); }

	void cpi() { compare(fetchByte());	}

	// rotate

	void rrc() {
		auto carry = a & 1;
		a >>= 1;
		a |= carry << 7;
		carry ? setCarry() : resetCarry();
	}

	// specials

	// input/output

	void out() {
		auto port = fetchByte();
		m_ports.write(port, a);
	}

	// control

	void ei() { m_interrupt = true; }
	void di() { m_interrupt = false; }

	void nop() {}

	void hlt() {
		m_halted = true;
	}
};
