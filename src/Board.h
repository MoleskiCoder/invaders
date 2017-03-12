#pragma once

#include <string>

#include "Memory.h"
#include "InputOutput.h"
#include "Configuration.h"
#include "Intel8080.h"

class Board {
public:
	Board(const Configuration& configuration);

	const Configuration& getConfiguration() const { return m_configuration; }
	Memory& getMemory() { return m_memory; }
	Intel8080& getCPU() { return m_cpu; }
	InputOutput& getIO() { return m_ports; }

	void initialise();

private:
	const Configuration& m_configuration;
	Memory m_memory;
	InputOutput m_ports;
	Intel8080 m_cpu;
};
