#pragma once

#include <cstdint>
#include <array>
#include <functional>
#include <bitset>

#include "Memory.h"
#include "InputOutput.h"

class Intel8080 {
public:

	typedef std::function<void()> instruction_t;

	struct Instruction {
		instruction_t vector = nullptr;
		uint64_t count = 0;
	};

	Intel8080(Memory& memory, InputOutput& ports);

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

	enum {
		F_C = 0x1,
		F_P = 0x4,
		F_AC = 0x10,
		F_Z = 0x40,
		F_S = 0x80
	};

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

	void resetFlags() {
		resetCarry();
		setFlag(0x2);
		resetParity();
		resetFlag(0x8);
		resetAuxiliaryCarry();
		resetFlag(0x20);
		resetZero();
		resetSign();
	}

	void pushByte(uint8_t value);
	uint8_t popByte();
	void pushWord(uint16_t value);
	uint16_t popWord();

	uint8_t fetchByte() { return m_memory.get(pc++); }
	uint16_t fetchWord() { return m_memory.getWord(pc); pc += 2;  }

	static Instruction INS(instruction_t method, uint64_t cycles);

	void installInstructions();

	//

	void compare(uint8_t value) {
		uint16_t subtraction = a - value;
		adjustSZP((uint8_t)subtraction);
		subtraction & 0x100 ? setCarry() : resetCarry();
	}

	//

	void ___();

	void nop();
	void jmp();
	void lxi_sp();
	void lxi_d();
	void lxi_h();
	void inr_h();
	void mvi_b();
	void mvi_c();
	void mvi_h();
	void mvi_m();
	void push_d();
	void push_h();
	void pop_h();
	void call();
	void ret();
	void ldax_d();
	void mov_m_a();
	void mov_a_h();
	void mov_e_a();
	void mov_e_h();
	void mov_l_a();
	void inx_d();
	void inx_h();
	void dcr_b();
	void jnz();
	void sta();
	void ani();
	void cpi();
	void dad_d();
	void dad_h();
	void xchg();
	void out();
};
