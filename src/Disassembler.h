#pragma once

#include <string>

class Disassembler {
public:
	Disassembler();

	static std::string hex(uint8_t value);
	static std::string hex(uint16_t value);
	static std::string binary(uint8_t value);

	static std::string invalid(uint8_t value);
};

