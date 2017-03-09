#pragma once

#include <string>

#include "Memory.h"
#include "Configuration.h"

class Board {
public:
	Board(const Configuration& configuration);
	~Board();

	const Configuration& getConfiguration() const { return m_configuration; }

	const Memory& getMemory() const { return m_memory; }
	Memory& getMemoryMutable() { return m_memory; }

	void initialise();

private:
	const Configuration& m_configuration;
	Memory m_memory;
};
