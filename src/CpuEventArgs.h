#pragma once

class Intel8080;

class CpuEventArgs {
public:
	CpuEventArgs(const Intel8080& cpu);

	const Intel8080& getCpu() const;

private:
	const Intel8080& m_cpu;
};
