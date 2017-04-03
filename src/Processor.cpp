#include "stdafx.h"
#include "Processor.h"

Processor::Processor(Memory& memory, InputOutput& ports)
:	m_memory(memory),
	m_ports(ports),
	cycles(0),
	pc(0),
	sp(0),
	m_halted(false) {}

void Processor::reset() {
	pc = 0;
}

void Processor::initialise() {
	sp = 0;
	reset();
}

void Processor::pushWord(uint16_t value) {
	sp -= 2;
	setWord(sp, value);
}

uint16_t Processor::popWord() {
	auto value = getWord(sp);
	sp += 2;
	return value;
}
