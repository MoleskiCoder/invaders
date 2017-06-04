#include "stdafx.h"
#include "Memory.h"
#include "Processor.h"

#include <iostream>
#include <fstream>
#include <algorithm>

Memory::Memory(uint16_t addressMask)
	: m_address(0),
	m_addressMask(addressMask),
	m_data(&(m_bus[m_address])) {}

uint8_t Memory::peek(uint16_t address) const {
	return m_bus[address];
}

uint16_t Memory::peekWord(uint16_t address) const {
	auto low = peek(address);
	auto high = peek(address + 1);
	return Processor::makeWord(low, high);
}

uint8_t Memory::get(uint16_t address) {
	ADDRESS() = address;
	return reference();
}

uint16_t Memory::getWord(uint16_t address) {
	auto low = get(address);
	auto high = get(address + 1);
	return Processor::makeWord(low, high);
}

void Memory::set(uint16_t address, uint8_t value) {
	ADDRESS() = address;
	reference() = value;
}

void Memory::setWord(uint16_t address, uint16_t value) {
	set(address, lowByte(value));
	set(address + 1, highByte(value));
}

uint8_t& Memory::reference() {
	uint16_t effective = ADDRESS() & m_addressMask;
	return m_locked[effective] ? placeDATA(m_bus[effective]) : referenceDATA(m_bus[effective]);
}

uint8_t& Memory::placeDATA(uint8_t value) {
	m_temporary = value;
	m_data = &m_temporary;
	return DATA();
}

uint8_t& Memory::referenceDATA(uint8_t& value) {
	m_data = &value;
	return DATA();
}

uint8_t& Memory::reference(uint16_t address) {
	ADDRESS() = address;
	return reference();
}

void Memory::clear() {
	m_bus.fill(0);
	m_locked.fill(false);
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
