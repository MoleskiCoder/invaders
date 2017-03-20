#include "stdafx.h"
#include "Memory.h"

#include <iostream>
#include <fstream>
#include <algorithm>

Memory::Memory(int addressMask)
: m_bus(0x10000),
  m_locked(m_bus.size()),
  m_addressMask(addressMask) {}

const std::vector<uint8_t>& Memory::getBus() const {
	return m_bus;
}

std::vector<uint8_t>& Memory::getBusMutable() {
	return m_bus;
}

uint8_t Memory::get(int address) const {
	return m_bus[address & m_addressMask];
}

uint16_t Memory::getWord(int address) const {
	auto low = get(address);
	auto high = get(address + 1);
	return makeWord(low, high);
}

void Memory::set(int address, uint8_t value) {
	if (!m_locked[address & m_addressMask])
		m_bus[address & m_addressMask] = value;
}

void Memory::setWord(int address, uint16_t value) {
	set(address, lowByte(value));
	set(address + 1, highByte(value));
}

void Memory::clear() {
	std::fill(m_bus.begin(), m_bus.end(), 0);
}

void Memory::loadRom(std::string path, uint16_t offset) {
	std::ifstream file;
	file.exceptions(std::ios::failbit | std::ios::badbit);

	file.open(path, std::ios::binary | std::ios::ate);
	auto size = (int)file.tellg();

	size_t extent = size + offset;

	if (extent > m_bus.size())
		throw std::runtime_error("ROM cannot fit");

	file.seekg(0, std::ios::beg);

	file.read((char*)&m_bus[offset], size);
	file.close();

	std::fill(m_locked.begin() + offset, m_locked.begin() + offset + size, true);
}
