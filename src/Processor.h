#pragma once

#include <cstdint>
#include <array>
#include <functional>

#include "Memory.h"
#include "InputOutput.h"
#include "Signal.h"
#include "CpuEventArgs.h"

class Processor {
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

	Processor(Memory& memory, InputOutput& ports);

	Signal<CpuEventArgs> ExecutingInstruction;

	const Memory& getMemory() const { return m_memory; }

	uint16_t getProgramCounter() const { return pc; }
	void setProgramCounter(uint16_t value) { pc = value; }

	uint16_t getStackPointer() const { return sp; }

	bool isHalted() const { return m_halted; }
	void halt() { m_halted = true; }

	virtual void initialise();

	void reset();

	uint16_t getWord(int address) const {
		auto low = m_memory.get(address);
		auto high = m_memory.get(address + 1);
		return makeWord(low, high);
	}

	void setWord(int address, uint16_t value) {
		m_memory.set(address, Memory::lowByte(value));
		m_memory.set(address + 1, Memory::highByte(value));
	}

protected:
	Memory& m_memory;
	InputOutput& m_ports;

	uint64_t cycles;

	uint16_t pc;
	uint16_t sp;

	bool m_halted;

	static uint16_t makeWord(uint8_t low, uint8_t high) {
		return (high << 8) + low;
	}

	void pushWord(uint16_t value);
	uint16_t popWord();

	uint8_t fetchByte() {
		return m_memory.get(pc++);
	}

	uint16_t fetchWord() {
		auto value = getWord(pc);
		pc += 2;
		return value;
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

	void jmpConditional(int conditional) {
		auto destination = fetchWord();
		if (conditional)
			pc = destination;
	}
};

