#pragma once

// Auxiliary carry logic from https://github.com/begoon/i8080-core

#include <cstdint>
#include <array>
#include <functional>

#include "Memory.h"
#include "StatusFlags.h"
#include "InputOutput.h"
#include "Signal.h"
#include "CpuEventArgs.h"

class Intel8080 {
public:

	typedef union {
		struct {
#ifdef HOST_LITTLE_ENDIAN
			uint8_t low;
			uint8_t high;
#endif
#ifdef HOST_BIG_ENDIAN
			uint8_t high;
			uint8_t low;
#endif
		};
		uint16_t word;
	} register16_t;

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

	Intel8080(Memory& memory, InputOutput& ports);

	Signal<CpuEventArgs> ExecutingInstruction;

	const std::array<Instruction, 0x100>& getInstructions() const { return instructions;  }
	const Memory& getMemory() const { return m_memory; }

	uint16_t getProgramCounter() const { return pc; }
	void setProgramCounter(uint16_t value) { pc = value; }

	uint16_t getStackPointer() const { return sp; }

	uint8_t getA() const { return a; }
	StatusFlags getF() const { return f; }

	const register16_t& getBC() const { return bc; }
	const register16_t& getDE() const { return de; }
	const register16_t& getHL() const { return hl; }

	bool isInterruptable() const {
		return m_interrupt;
	}

	void disableInterrupts() { m_interrupt = false; }
	void enableInterrupts() { m_interrupt = true; }

	void interrupt(uint8_t vector) {
		if (isInterruptable()) {
			disableInterrupts();
			restart(vector);
		}
	}

	bool isHalted() const { return m_halted; }
	void halt() { m_halted = true; }

	void initialise();

	void reset();
	void step();

private:
	std::array<Instruction, 0x100> instructions;

	std::array<bool, 8> m_halfCarryTableAdd = { { false, false, true, false, true, false, true, true } };
	std::array<bool, 8> m_halfCarryTableSub = { { false, true, true, true, false, false, false, true } };

	Memory& m_memory;
	InputOutput& m_ports;

	uint64_t cycles;

	uint16_t pc;
	uint16_t sp;

	uint8_t a;
	StatusFlags f;

	register16_t bc;
	register16_t de;
	register16_t hl;

	bool m_interrupt;
	bool m_halted;

	void adjustSign(uint8_t value) { f.S = ((value & 0x80) != 0); }
	void adjustZero(uint8_t value) { f.Z = (value == 0); }

	void adjustParity(uint8_t value) {
		static const uint8_t lookup[0x10] = { 0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4 };
		auto set = (lookup[value >> 4] + lookup[value & 0xF]);
		f.P = (set % 2) == 0;
	}

	void adjustSZP(uint8_t value) {
		adjustSign(value);
		adjustZero(value);
		adjustParity(value);
	}

	int buildAuxiliaryCarryIndex(uint8_t value, int calculation) {
		return ((a & 0x88) >> 1) | ((value & 0x88) >> 2) | ((calculation & 0x88) >> 3);
	}

	void adjustAuxiliaryCarryAdd(uint8_t value, int calculation) {
		auto index = buildAuxiliaryCarryIndex(value, calculation);
		f.AC = m_halfCarryTableAdd[index & 0x7];
	}

	void adjustAuxiliaryCarrySub(uint8_t value, int calculation) {
		auto index = buildAuxiliaryCarryIndex(value, calculation);
		f.AC = !m_halfCarryTableSub[index & 0x7];
	}

	void postIncrement(uint8_t value) {
		adjustSZP(value);
		f.AC = (value & 0x0f) == 0;
	}

	void postDecrement(uint8_t value) {
		adjustSZP(value);
		f.AC = (value & 0x0f) != 0xf;
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
		adjustAuxiliaryCarrySub(value, subtraction);
		f.C = subtraction > 0xff;
	}

	void callAddress(uint16_t address) {
		pushWord(pc + 2);
		pc = address;
	}

	void restart(uint8_t position) {
		uint16_t address = position << 3;
		pushWord(pc);
		pc = address;
	}

	void callConditional(int condition) {
		if (condition) {
			call();
			cycles += 6;
		} else {
			pc += 2;
		}
	}

	void returnConditional(int condition) {
		if (condition) {
			ret();
			cycles += 6;
		}
	}

	void jmpConditional(int conditional) {
		auto destination = fetchWord();
		if (conditional)
			pc = destination;
	}

	void anda(uint8_t value) {
		f.AC = (((a | value) & 0x8) != 0);
		f.C = false;
		adjustSZP(a &= value);
	}

	void ora(uint8_t value) {
		f.AC = f.C = false;
		adjustSZP(a |= value);
	}

	void xra(uint8_t value) {
		f.AC = f.C = false;
		adjustSZP(a ^= value);
	}

	void add(uint8_t value) {
		uint16_t sum = a + value;
		a = Memory::lowByte(sum);
		f.C = sum > 0xff;
		adjustSZP(a);
		adjustAuxiliaryCarryAdd(value, sum);
	}

	void adc(uint8_t value) {
		add(value + f.C);
	}

	void dad(uint16_t value) {
		uint32_t sum = hl.word + value;
		f.C = sum > 0xffff;
		hl.word = (uint16_t)sum;
	}

	void sub(uint8_t value) {
		uint16_t difference = a - value;
		a = Memory::lowByte(difference);
		f.C = difference > 0xff;
		adjustSZP(a);
		adjustAuxiliaryCarrySub(value, difference);
	}

	void sbb(uint8_t value) {
		sub(value + f.C);
	}

	void mov_m_r(uint8_t value) {
		m_memory.set(hl.word, value);
	}

	uint8_t mov_r_m() {
		return m_memory.get(hl.word);
	}

	//

	void ___();

	// Move, load, and store

	void mov_a_a() { }
	void mov_a_b() { a = bc.high; }
	void mov_a_c() { a = bc.low; }
	void mov_a_d() { a = de.high; }
	void mov_a_e() { a = de.low; }
	void mov_a_h() { a = hl.high; }
	void mov_a_l() { a = hl.low; }

	void mov_b_a() { bc.high = a; }
	void mov_b_b() { }
	void mov_b_c() { bc.high = bc.low; }
	void mov_b_d() { bc.high = de.high; }
	void mov_b_e() { bc.high = de.low; }
	void mov_b_h() { bc.high = hl.high; }
	void mov_b_l() { bc.high = hl.low; }

	void mov_c_a() { bc.low = a; }
	void mov_c_b() { bc.low = bc.high; }
	void mov_c_c() { }
	void mov_c_d() { bc.low = de.high; }
	void mov_c_e() { bc.low = de.low; }
	void mov_c_h() { bc.low = hl.high; }
	void mov_c_l() { bc.low = hl.low; }

	void mov_d_a() { de.high = a; }
	void mov_d_b() { de.high = bc.high; }
	void mov_d_c() { de.high = bc.low; }
	void mov_d_d() { }
	void mov_d_e() { de.high = de.low; }
	void mov_d_h() { de.high = hl.high; }
	void mov_d_l() { de.high = hl.low; }

	void mov_e_a() { de.low = a; }
	void mov_e_b() { de.low = bc.high; }
	void mov_e_c() { de.low = bc.low; }
	void mov_e_d() { de.low = de.high; }
	void mov_e_e() { }
	void mov_e_h() { de.low = hl.high; }
	void mov_e_l() { de.low = hl.low; }

	void mov_h_a() { hl.high = a; }
	void mov_h_b() { hl.high = bc.high; }
	void mov_h_c() { hl.high = bc.low; }
	void mov_h_d() { hl.high = de.high; }
	void mov_h_e() { hl.high = de.low; }
	void mov_h_h() { }
	void mov_h_l() { hl.high = hl.low; }

	void mov_l_a() { hl.low = a; }
	void mov_l_b() { hl.low = bc.high; }
	void mov_l_c() { hl.low = bc.low; }
	void mov_l_d() { hl.low = de.high; }
	void mov_l_e() { hl.low = de.low; }
	void mov_l_h() { hl.low = hl.high; }
	void mov_l_l() { }

	void mov_m_a() { mov_m_r(a); }
	void mov_m_b() { mov_m_r(bc.high); }
	void mov_m_c() { mov_m_r(bc.low); }
	void mov_m_d() { mov_m_r(de.high); }
	void mov_m_e() { mov_m_r(de.low); }
	void mov_m_h() { mov_m_r(hl.high); }
	void mov_m_l() { mov_m_r(hl.low); }

	void mov_a_m() { a = mov_r_m(); }
	void mov_b_m() { bc.high = mov_r_m(); }
	void mov_c_m() { bc.low = mov_r_m(); }
	void mov_d_m() { de.high = mov_r_m(); }
	void mov_e_m() { de.low = mov_r_m(); }
	void mov_h_m() { hl.high = mov_r_m(); }
	void mov_l_m() { hl.low = mov_r_m(); }

	void mvi_a() { a = fetchByte(); }
	void mvi_b() { bc.high = fetchByte(); }
	void mvi_c() { bc.low = fetchByte(); }
	void mvi_d() { de.high = fetchByte(); }
	void mvi_e() { de.low = fetchByte(); }
	void mvi_h() { hl.high = fetchByte(); }
	void mvi_l() { hl.low = fetchByte(); }

	void mvi_m() {
		auto data = fetchByte();
		m_memory.set(hl.word, data);
	}

	void lxi_b() { bc.word = fetchWord(); }
	void lxi_d() { de.word = fetchWord(); }
	void lxi_h() { hl.word = fetchWord(); }

	void stax_b() { m_memory.set(bc.word, a); }
	void stax_d() { m_memory.set(de.word, a); }

	void ldax_b() { a = m_memory.get(bc.word); }
	void ldax_d() { a = m_memory.get(de.word); }

	void sta() {
		auto destination = fetchWord();
		m_memory.set(destination, a);
	}

	void lda() {
		auto source = fetchWord();
		a = m_memory.get(source);
	}

	void shld() {
		auto destination = fetchWord();
		m_memory.setWord(destination, hl.word);
	}

	void lhld() {
		auto source = fetchWord();
		hl.word = m_memory.getWord(source);
	}

	void xchg() {
		std::swap(de, hl);
	}

	// stack ops

	void push_b() { pushWord(bc.word); }
	void push_d() { pushWord(de.word); }
	void push_h() { pushWord(hl.word); }

	void push_psw() {
		auto pair = Memory::makeWord(f, a);
		pushWord(pair);
	}

	void pop_b() { bc.word = popWord(); }
	void pop_d() { de.word = popWord(); }
	void pop_h() { hl.word = popWord(); }

	void pop_psw() {
		auto af = popWord();
		a = Memory::highByte(af);
		f = Memory::lowByte(af);
	}

	void xhtl() {
		auto tos = m_memory.getWord(sp);
		m_memory.setWord(sp, hl.word);
		hl.word = tos;
	}

	void sphl() {
		sp = hl.word;
	}

	void lxi_sp() {
		sp = fetchWord();
	}

	void inx_sp() { ++sp; }
	void dcx_sp() { --sp; }

	// jump

	void jmp() { jmpConditional(true); }
	
	void jc() { jmpConditional(f.C); }
	void jnc() { jmpConditional(!f.C); }
	
	void jz() { jmpConditional(f.Z); }
	void jnz() { jmpConditional(!f.Z); }
	
	void jpe() { jmpConditional(f.P); }
	void jpo() { jmpConditional(!f.P); }
	
	void jm() { jmpConditional(f.S); }
	void jp() { jmpConditional(!f.S); }
	
	void pchl() {
		pc = hl.word;
	}

	// call

	void call() {
		auto destination = m_memory.getWord(pc);
		callAddress(destination);
	}

	void cc() { callConditional(f.C); }
	void cnc() { callConditional(!f.C); }

	void cpe() { callConditional(f.P); }
	void cpo() { callConditional(!f.P);  }

	void cz() { callConditional(f.Z); }
	void cnz() { callConditional(!f.Z); }

	void cm() { callConditional(f.S); }
	void cp() { callConditional(!f.S); }

	// return

	void ret() {
		pc = popWord();
	}

	void rc() { returnConditional(f.C); }
	void rnc() { returnConditional(!f.C); }

	void rz() { returnConditional(f.Z); }
	void rnz() { returnConditional(!f.Z); }

	void rpe() { returnConditional(f.P); }
	void rpo() { returnConditional(!f.P); }

	void rm() { returnConditional(f.S); }
	void rp() { returnConditional(!f.S); }

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

	void inr_a() { postIncrement(++a); }
	void inr_b() { postIncrement(++bc.high); }
	void inr_c() { postIncrement(++bc.low); }
	void inr_d() { postIncrement(++de.high); }
	void inr_e() { postIncrement(++de.low); }
	void inr_h() { postIncrement(++hl.high); }
	void inr_l() { postIncrement(++hl.low); }

	void inr_m() {
		auto value = m_memory.get(hl.word);
		postIncrement(++value);
		m_memory.set(hl.word, value);
	}

	void dcr_a() { postDecrement(--a); }
	void dcr_b() { postDecrement(--bc.high); }
	void dcr_c() { postDecrement(--bc.low); }
	void dcr_d() { postDecrement(--de.high); }
	void dcr_e() { postDecrement(--de.low); }
	void dcr_h() { postDecrement(--hl.high); }
	void dcr_l() { postDecrement(--hl.low); }

	void dcr_m() {
		auto value = m_memory.get(hl.word);
		postDecrement(--value);
		m_memory.set(hl.word, value);
	}

	void inx_b() { ++bc.word; }
	void inx_d() { ++de.word; }
	void inx_h() { ++hl.word; }

	void dcx_b() { --bc.word; }
	void dcx_d() { --de.word; }
	void dcx_h() { --hl.word; }

	// add

	void add_a() { add(a); }
	void add_b() { add(bc.high); }
	void add_c() { add(bc.low); }
	void add_d() { add(de.high); }
	void add_e() { add(de.low); }
	void add_h() { add(hl.high); }
	void add_l() { add(hl.low); }

	void add_m() {
		auto value = m_memory.get(hl.word);
		add(value);
	}

	void adi() { add(fetchByte()); }

	void adc_a() { adc(a); }
	void adc_b() { adc(bc.high); }
	void adc_c() { adc(bc.low); }
	void adc_d() { adc(de.high); }
	void adc_e() { adc(de.low); }
	void adc_h() { adc(hl.high); }
	void adc_l() { adc(hl.low); }

	void adc_m() {
		auto value = m_memory.get(hl.word);
		adc(value);
	}

	void aci() { adc(fetchByte()); }

	void dad_b() { dad(bc.word); }
	void dad_d() { dad(de.word); }
	void dad_h() { dad(hl.word); }
	void dad_sp() { dad(sp); }

	// subtract

	void sub_a() { sub(a); }
	void sub_b() { sub(bc.high); }
	void sub_c() { sub(bc.low); }
	void sub_d() { sub(de.high); }
	void sub_e() { sub(de.low); }
	void sub_h() { sub(hl.high); }
	void sub_l() { sub(hl.low); }

	void sub_m() {
		auto value = m_memory.get(hl.word);
		sub(value);
	}

	void sbb_a() { sbb(a); }
	void sbb_b() { sbb(bc.high); }
	void sbb_c() { sbb(bc.low); }
	void sbb_d() { sbb(de.high); }
	void sbb_e() { sbb(de.low); }
	void sbb_h() { sbb(hl.high); }
	void sbb_l() { sbb(hl.low); }

	void sbb_m() {
		auto value = m_memory.get(hl.word);
		sbb(value);
	}

	void sbi() {
		auto value = fetchByte();
		sbb(value);
	}

	void sui() {
		auto value = fetchByte();
		sub(value);
	}

	// logical

	void ana_a() { anda(a); }
	void ana_b() { anda(bc.high); }
	void ana_c() { anda(bc.low); }
	void ana_d() { anda(de.high); }
	void ana_e() { anda(de.low); }
	void ana_h() { anda(hl.high); }
	void ana_l() { anda(hl.low); }

	void ana_m() {
		auto value = m_memory.get(hl.word);
		anda(value);
	}

	void ani() { anda(fetchByte()); }

	void xra_a() { xra(a); }
	void xra_b() { xra(bc.high); }
	void xra_c() { xra(bc.low); }
	void xra_d() { xra(de.high); }
	void xra_e() { xra(de.low); }
	void xra_h() { xra(hl.high); }
	void xra_l() { xra(hl.low); }

	void xra_m() {
		auto value = m_memory.get(hl.word);
		xra(value);
	}

	void xri() { xra(fetchByte()); }

	void ora_a() { ora(a); }
	void ora_b() { ora(bc.high); }
	void ora_c() { ora(bc.low); }
	void ora_d() { ora(de.high); }
	void ora_e() { ora(de.low); }
	void ora_h() { ora(hl.high); }
	void ora_l() { ora(hl.low); }

	void ora_m() {
		auto value = m_memory.get(hl.word);
		ora(value);
	}

	void ori() { ora(fetchByte()); }

	void cmp_a() { compare(a); }
	void cmp_b() { compare(bc.high); }
	void cmp_c() { compare(bc.low); }
	void cmp_d() { compare(de.high); }
	void cmp_e() { compare(de.low); }
	void cmp_h() { compare(hl.high); }
	void cmp_l() { compare(hl.low); }

	void cmp_m() {
		auto value = m_memory.get(hl.word);
		compare(value);
	}

	void cpi() { compare(fetchByte());	}

	// rotate

	void rlc() {
		auto carry = a & 0x80;
		a <<= 1;
		a |= carry >> 7;
		f.C = carry != 0;
	}

	void rrc() {
		auto carry = a & 1;
		a >>= 1;
		a |= carry << 7;
		f.C = carry != 0;
	}

	void ral() {
		auto carry = a & 0x80;
		a <<= 1;
		a |= (uint8_t)f.C;
		f.C = carry != 0;
	}

	void rar() {
		auto carry = a & 1;
		a >>= 1;
		a |= f.C << 7;
		f.C = carry != 0;
	}

	// specials

	void cma() {
		a ^= 0xff;
	}

	void stc() {
		f.C = true;
	}

	void cmc() {
		f.C = !f.C;
	}

	void daa() {
		auto carry = f.C;
		uint8_t addition = 0;
		if (f.AC || (a & 0xf) > 9) {
			addition = 0x6;
		}
		if (f.C || (a >> 4) > 9 || ((a >> 4) >= 9 && (a & 0xf) > 9)) {
			addition |= 0x60;
			carry = true;
		}
		add(addition);
		f.C = carry;
	}

	// input/output

	void out() {
		auto port = fetchByte();
		m_ports.write(port, a);
	}

	void in() {
		auto port = fetchByte();
		a = m_ports.read(port);
	}

	// control

	void ei() { enableInterrupts(); }
	void di() { disableInterrupts(); }

	void nop() {}

	void hlt() {
		m_halted = true;
	}
};
