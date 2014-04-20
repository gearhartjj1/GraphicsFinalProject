#include "inputLine.h"

InputLine::InputLine(QWidget* parent = 0) : QLineEdit(parent) {
}

InputLine::~InputLine() {
}

void InputLine::signalFile() {
	emit sendText(text());
}