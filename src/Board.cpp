#include "stdafx.h"
#include "Board.h"

Board::Board(const Configuration& configuration)
: m_configuration(configuration) {
}

Board::~Board() {
}

void Board::initialise() {
	auto romDirectory = m_configuration.getRomDirectory();
	m_memory.loadRom(romDirectory + "/invaders.e", 0x1800);
	m_memory.loadRom(romDirectory + "/invaders.f", 0x1000);
	m_memory.loadRom(romDirectory + "/invaders.g", 0x0800);
	m_memory.loadRom(romDirectory + "/invaders.h", 0x0000);
}