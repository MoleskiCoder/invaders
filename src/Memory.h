#pragma once

#include <vector>
#include <cstdint>
#include <string>

class Memory {
public:
	enum {
		WorkRam = 0x2000,
		VideoRam = 0x2400
	};

	static uint8_t lowNybble(uint8_t value) { return value & 0xf; }
	static uint8_t highNybble(uint8_t value) { return demoteNybble(value); }

	static uint8_t promoteNybble(uint8_t value) { return value << 4; }
	static uint8_t demoteNybble(uint8_t value) { return value >> 4; }

	static uint8_t lowByte(uint16_t value) { return value & 0xff; }
	static uint8_t highByte(uint16_t value) { return value >> 8; }

	static uint16_t makeWord(uint8_t low, uint8_t high) { return (high << 8) + low; }

	Memory(int addressMask);

	std::vector<uint8_t>& getBusMutable();

	uint8_t get(int address) const;
	uint16_t getWord(int address) const;

	void set(int address, uint8_t value);
	void setWord(int address, uint16_t value);

	void clear();
	void loadRom(std::string path, uint16_t offset);

private:
	std::vector<uint8_t> m_bus;
	std::vector<bool> m_locked;

	int m_addressMask;		// Mirror
};
