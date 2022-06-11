#include "ConsoleOutput.h"

ConsoleOutput::ConsoleOutput()
{
	handle = GetStdHandle(STD_OUTPUT_HANDLE);
	TurnOffCursor();
}

void ConsoleOutput::DrawCell(int x, int y, int number)
{
	byte color = NumberToColor(number);
	this->SetColor(color);

	std::string fill = "  ";
	SetCursorPosition(x * fill.length(), y);
	printf(fill.c_str());

	this->SetColor(COLORS::BLACK);
}

void ConsoleOutput::SetCursorPosition(int x, int y) {
	COORD pos = { x, y };
	SetConsoleCursorPosition(handle, pos);
}

void ConsoleOutput::SetColor(byte color)
{
	SetConsoleTextAttribute(handle, color);
}

const byte ConsoleOutput::NumberToColor(int number) const
{
	switch (number) {
	case 0:
		return COLORS::BLUE;
	case 1:
		return COLORS::WHITE;
	case 2:
		return COLORS::GREEN;
	case 3:
		return COLORS::TURQUOISE;
	case 4:
		return COLORS::YELLOW;
	case 5:
		return COLORS::GOLD;
	default:
		return COLORS::RED;
	}
}

void ConsoleOutput::TurnOffCursor()
{
	CONSOLE_CURSOR_INFO     cursorInfo;

	GetConsoleCursorInfo(handle, &cursorInfo);
	cursorInfo.bVisible = false;
	SetConsoleCursorInfo(handle, &cursorInfo);
}