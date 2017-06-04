#pragma once

#include <cstdint>
#include <array>
#include <functional>

#include "Memory.h"
#include "InputOutput.h"
#include "Signal.h"

class Processor {
public:
	enum Masks {
		Mask1 = 0x01,
		Mask2 = 0x03,
		Mask3 = 0x07,
		Mask4 = 0x0f,
		Mask5 = 0x1f,
		Mask6 = 0x3f,
		Mask7 = 0x7f,
		Mask8 = 0xff,
	};

	enum Bits {
		Bit16 = 0x10000,
		Bit15 = 0x8000,
		Bit14 = 0x4000,
		Bit13 = 0x2000,
		Bit12 = 0x1000,
		Bit11 = 0x800,
		Bit10 = 0x400,
		Bit9 = 0x200,
		Bit8 = 0x100,
		Bit7 = 0x80,
		Bit6 = 0x40,
		Bit5 = 0x20,
		Bit4 = 0x10,
		Bit3 = 0x8,
		Bit2 = 0x4,
		Bit1 = 0x2,
		Bit0 = 0x1,
	};

	static uint8_t highNibble(uint8_t value) { return value >> 4; }
	static uint8_t lowNibble(uint8_t value) { return value & Mask4; }

	static uint8_t promoteNibble(uint8_t value) { return value << 4; }
	static uint8_t demoteNibble(uint8_t value) { return highNibble(value); }

	static uint16_t makeWord(uint8_t low, uint8_t high) {
		return (high << 8) | low;
	}

	Processor(Memory& memory, InputOutput& ports);

	const Memory& getMemory() const { return m_memory; }

	register16_t getProgramCounter() const { return pc; }
	void setProgramCounter(register16_t value) { pc = value; }

	register16_t getStackPointer() const { return sp; }
	void setStackPointer(register16_t value) { sp = value; }

	bool isHalted() const { return m_halted; }
	void halt() { --pc.word;  m_halted = true; }

	uint64_t getCycles() const { return cycles; }

	virtual void initialise();

	void reset();

	virtual register16_t getWord(uint16_t address) const {
		return m_memory.getWord(address);
	}

	virtual void setWord(uint16_t address, register16_t value) {
		m_memory.setWord(address, value);
	}

protected:
	Memory& m_memory;
	InputOutput& m_ports;

	int cycles;

	register16_t pc;
	register16_t sp;

	bool m_halted;

	void pushWord(register16_t value);
	register16_t popWord();

	uint8_t fetchByte() {
		return m_memory.get(pc.word++);
	}

	uint16_t fetchWord() {
		auto value = getWord(pc.word);
		pc.word += 2;
		return value.word;
	}
};
