#pragma once

#include <cstdint>
#include <array>

class Memory;

class Intel8080 {
public:
	Intel8080(Memory& memory);

	void initialise();

	void reset();

	const std::array<uint8_t, 0x100>& getPorts() const { return ports; }
	std::array<uint8_t, 0x100>& getPortsMutable() { return ports; }

private:
	Memory& m_memory;
	std::array<uint8_t, 0x100> ports;

	uint16_t pc;
	uint16_t sp;

	uint8_t a;

	uint8_t b;
	uint8_t c;

	uint8_t d;
	uint8_t e;

	uint8_t h;
	uint8_t l;
};
