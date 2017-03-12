#include "stdafx.h"
#include "InputOutput.h"

InputOutput::InputOutput() {
}

uint8_t InputOutput::readInputPort(uint8_t port) const {
	ReadingPort.fire(PortEventArgs(port));
	return input[port];
	ReadPort.fire(PortEventArgs(port));
}

void InputOutput::writeOutputPort(uint8_t port, uint8_t value) {
	WritingPort.fire(PortEventArgs(port));
	output[port] = value;
	WrittenPort.fire(PortEventArgs(port));
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
