#include "stdafx.h"
#include "Memory.h"

#include <iostream>
#include <fstream>
#include <algorithm>

Memory::Memory(int addressMask)
: m_addressMask(addressMask) {}

uint8_t Memory::get(int address) const {
	return m_bus[address & m_addressMask];
}

void Memory::set(int address, uint8_t value) {
	if (!m_locked[address & m_addressMask])
		m_bus[address & m_addressMask] = value;
}

void Memory::clear() {
	std::fill(m_bus.begin(), m_bus.end(), 0);
}

void Memory::loadRom(const std::string& path, uint16_t offset) {
	auto size = loadMemory(path, offset);
	std::fill(m_locked.begin() + offset, m_locked.begin() + offset + size, true);
}

void Memory::loadRam(const std::string& path, uint16_t offset) {
	loadMemory(path, offset);
}

int Memory::loadMemory(const std::string& path, uint16_t offset) {
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

	return size;
}
