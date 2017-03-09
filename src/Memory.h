#pragma once

#include <vector>

class Memory {
public:
	Memory();

	const std::vector<uint8_t>& getBus() const;
	std::vector<uint8_t>& getBusMutable();

	uint8_t get(int address) const;
	uint16_t getWord(int address) const;

	void set(int address, uint8_t value);
	void setWord(int address, uint16_t value);

	void clear();
	void loadRom(std::string path, uint16_t offset);

private:
	std::vector<uint8_t> m_bus;
};

