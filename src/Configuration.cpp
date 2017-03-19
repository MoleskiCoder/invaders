#include "stdafx.h"
#include "Configuration.h"

Configuration::Configuration()
:	m_debugMode(true),
	m_vsyncLocked(true),
	m_framesPerSecond(60),
	m_cyclesPerSecond(2000000),
	m_romDirectory("roms"),
	m_machineMode(SpaceInvaders),
	m_startAddress(0) {
}
