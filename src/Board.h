#pragma once

#include <string>

#include "Memory.h"
#include "InputOutput.h"
#include "Configuration.h"
#include "Intel8080.h"

class Board {
public:
	Board(const Configuration& configuration);
	~Board();

	const Configuration& getConfiguration() const { return m_configuration; }

	const Memory& getMemory() const { return m_memory; }
	Memory& getMemoryMutable() { return m_memory; }

	Intel8080& getCPU() { return m_cpu; }

	void initialise();

private:
	const Configuration& m_configuration;
	Memory m_memory;
	InputOutput m_ports;
	Intel8080 m_cpu;
};
