#include "stdafx.h"
#include "Intel8080.h"

CpuEventArgs::CpuEventArgs(const Intel8080& cpu)
: m_cpu(cpu) {}

const Intel8080& CpuEventArgs::getCpu() const {
	return m_cpu;
}
