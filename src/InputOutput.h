#pragma once

#include <cstdint>

class InputOutput {
public:
	InputOutput();

	uint8_t read(uint8_t port) const { return readInputPort(port); }
	void write(uint8_t port, uint8_t value) { return writeOutputPort(port, value); }

	uint8_t readInputPort(uint8_t port) const { return input[port]; }
	void writeInputPort(uint8_t port, uint8_t value) { input[port] = value;  }

	uint8_t readOutputPort(uint8_t port) const { return output[port]; }
	void writeOutputPort(uint8_t port, uint8_t value) { output[port] = value; }

private:
	std::array<uint8_t, 0x100> input;
	std::array<uint8_t, 0x100> output;
};
