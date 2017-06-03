#pragma once

#include <array>
#include <cstdint>
#include <string>

class Memory {
public:
	static uint8_t lowByte(uint16_t value) { return value & 0xff; }
	static uint8_t highByte(uint16_t value) { return value >> 8; }

	Memory(uint16_t addressMask);

	virtual uint16_t& ADDRESS() { return m_address; }
	virtual uint8_t& DATA() { return *m_data; }

	virtual uint8_t& placeDATA(uint8_t value);
	virtual uint8_t& referenceDATA(uint8_t& value);

	virtual uint8_t peek(uint16_t address) const;
	virtual uint16_t peekWord(uint16_t address) const;

	virtual uint8_t get(uint16_t address);
	virtual uint16_t getWord(uint16_t address);

	virtual void set(uint16_t address, uint8_t value);
	virtual void setWord(uint16_t address, uint16_t value);

	virtual uint8_t& reference();
	virtual uint8_t& reference(uint16_t address);

	void clear();
	void loadRom(const std::string& path, uint16_t offset);
	void loadRam(const std::string& path, uint16_t offset);

protected:
	std::array<uint8_t, 0x10000> m_bus;
	std::array<bool, 0x10000> m_locked;

	uint16_t m_addressMask;		// Mirror
	uint8_t m_temporary;	// Used to simulate ROM
	uint16_t m_address;
	uint8_t* m_data;

	int loadMemory(const std::string& path, uint16_t offset);
};
