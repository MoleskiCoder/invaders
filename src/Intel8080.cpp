#include "stdafx.h"
#include "Intel8080.h"

#include "Memory.h"

Intel8080::Intel8080(Memory& memory)
: m_memory(memory) {
}

void Intel8080::reset() {
	pc = 0;
}

void Intel8080::initialise() {
	m_memory.clear();
	pc = sp = 0;
	a = b = c = d = e = h = l = 0;
	reset();
}
