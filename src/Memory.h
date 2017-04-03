#pragma once

#include <array>
#include <cstdint>
#include <string>

class Memory {
public:
	static uint8_t lowByte(uint16_t value) { return value & 0xff; }
	static uint8_t highByte(uint16_t value) { return value >> 8; }

	Memory(int addressMask);

	uint8_t get(int address) const;
	void set(int address, uint8_t value);

	void clear();
	void loadRom(const std::string& path, uint16_t offset);
	void loadRam(const std::string& path, uint16_t offset);

private:
	std::array<uint8_t, 0x10000> m_bus;
	std::array<bool, 0x10000> m_locked;

	int m_addressMask;		// Mirror

	int loadMemory(const std::string& path, uint16_t offset);
};
