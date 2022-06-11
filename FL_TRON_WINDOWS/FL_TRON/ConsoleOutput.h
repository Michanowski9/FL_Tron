#ifndef _CONSOLE_OUTPUT_H_
#define _CONSOLE_OUTPUT_H_

#include "GraphicsEngine.h"

#include <cstdio>
#include <string>
#include <Windows.h>

class ConsoleOutput : public GraphicsEngine
{
public:
	ConsoleOutput();
	~ConsoleOutput() = default;

	void DrawCell(int x, int y, int cell = 0) override;

private:
	void SetCursorPosition(int x, int y);
	void SetColor(byte color);
	const byte NumberToColor(int number) const;
	void TurnOffCursor();

	HANDLE handle;
	enum COLORS {
		BLACK = 15,
		DARK_BLUE = 31,
		GREEN = 47,
		BLUE = 63,
		RED = 79,
		DARK_PURPLE = 95,
		GOLD = 111,
		LIGHT_GREY = 112,
		DARK_GREY = 143,
		LIGHT_BLUE = 159,
		LIGHT_GREEN = 175,
		TURQUOISE = 191,
		LIGHT_RED = 207,
		LIGHT_PURPLE = 223,
		YELLOW = 224,
		WHITE = 240
	};
};

#endif // !_CONSOLE_OUTPUT_H_