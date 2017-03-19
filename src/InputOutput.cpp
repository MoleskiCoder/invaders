#include "stdafx.h"
#include "InputOutput.h"

InputOutput::InputOutput() {
}

uint8_t InputOutput::readInputPort(uint8_t port) {
	OnReadingPort(port);
	auto value = input[port];
	OnReadPort(port);
	return value;
}

void InputOutput::writeOutputPort(uint8_t port, uint8_t value) {
	OnWritingPort(port);
	output[port] = value;
	OnWrittenPort(port);
}

void InputOutput::OnReadingPort(uint8_t port) {
	ReadingPort.fire(PortEventArgs(port));
}

void InputOutput::OnReadPort(uint8_t port) {
	ReadPort.fire(PortEventArgs(port));
}

void InputOutput::OnWritingPort(uint8_t port) {
	WritingPort.fire(PortEventArgs(port));
}

void InputOutput::OnWrittenPort(uint8_t port) {
	WrittenPort.fire(PortEventArgs(port));
}
