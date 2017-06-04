#include "stdafx.h"
#include "Processor.h"

Processor::Processor(Memory& memory, InputOutput& ports)
:	m_memory(memory),
	m_ports(ports),
	cycles(0),
	m_halted(false) {
	pc.word = sp.word = 0;
}

void Processor::reset() {
	pc.word = 0;
}

void Processor::initialise() {
	sp.word = 0;
	reset();
}

void Processor::pushWord(register16_t value) {
	sp.word -= 2;
	setWord(sp.word, value);
}

register16_t Processor::popWord() {
	auto value = getWord(sp.word);
	sp.word += 2;
	return value;
}
