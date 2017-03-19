#pragma once

#include <string>

#include "Memory.h"
#include "InputOutput.h"
#include "Configuration.h"
#include "Intel8080.h"
#include "Profiler.h"

class Board {
public:
	Board(const Configuration& configuration);

	const Configuration& getConfiguration() const { return m_configuration; }
	Memory& getMemory() { return m_memory; }
	const Intel8080& getCPU() const { return m_cpu; }
	Intel8080& getCPUMutable() { return m_cpu; }
	InputOutput& getIO() { return m_ports; }

	void initialise();

private:
	const Configuration& m_configuration;
	Memory m_memory;
	InputOutput m_ports;
	Intel8080 m_cpu;
	Profiler m_profiler;

	void Cpu_ExecutingInstruction_Cpm(const CpuEventArgs& cpuEvent);

	void Board_PortWritten(const PortEventArgs& portEvent);
	void Board_PortRead(const PortEventArgs& portEvent);

	void Cpu_ExecutingInstruction_Debug(const CpuEventArgs& cpuEvent);

	void bdos();
};
