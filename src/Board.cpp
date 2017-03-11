#include "stdafx.h"
#include "Board.h"

Board::Board(const Configuration& configuration)
: m_configuration(configuration),
  m_cpu(Intel8080(m_memory)) {
}

Board::~Board() {
}

void Board::initialise() {

	m_memory.clear();
	auto romDirectory = m_configuration.getRomDirectory();
	m_memory.loadRom(romDirectory + "/invaders.e", 0x1800);
	m_memory.loadRom(romDirectory + "/invaders.f", 0x1000);
	m_memory.loadRom(romDirectory + "/invaders.g", 0x0800);
	m_memory.loadRom(romDirectory + "/invaders.h", 0x0000);

	m_cpu.initialise();
}