#pragma once

#include <cstdint>
#include <array>
#include <functional>

class Memory;

class Intel8080 {
public:

	typedef std::function<void()> instruction_t;

	struct Instruction {
		instruction_t vector = nullptr;
		uint64_t count = 0;
	};

	Intel8080(Memory& memory);

	void initialise();

	void reset();

	const std::array<uint8_t, 0x100>& getPorts() const { return ports; }
	std::array<uint8_t, 0x100>& getPortsMutable() { return ports; }

	void step();

private:
	std::array<Instruction, 0x100> instructions;

	Memory& m_memory;
	std::array<uint8_t, 0x100> ports;

	uint64_t cycles;

	uint16_t pc;
	uint16_t sp;

	uint8_t a;

	uint8_t b;
	uint8_t c;

	uint8_t d;
	uint8_t e;

	uint8_t h;
	uint8_t l;

	void pushByte(uint8_t value);
	uint8_t popByte();
	void pushWord(uint16_t value);
	uint16_t popWord();

	static Instruction INS(instruction_t method, uint64_t cycles);

	void installInstructions();

	void ___();

	void nop();
	void jmp();
	void lxi_sp();
	void lxi_d();
	void lxi_h();
	void inr_h();
	void mvi_b();
	void call();
	void ldax_d();
	void mov_m_a();
	void inx_d();
	void inx_h();
	void dcr_b();
};
