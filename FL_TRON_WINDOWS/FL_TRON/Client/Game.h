#ifndef _GAME_H_
#define _GAME_H_

#include <mutex>
#include "../structs.h"

#include "InputHandler.h"
#include "GraphicsEngine.h"
#include <queue>
#include <thread>

#include "connect.h"

struct Point {
	Point(int x, int y) : x(x), y(y) {}
	int x;
	int y;
};

namespace KEY {
	const char UP = 0x26;
	const char DOWN = 0x28;
	const char LEFT = 0x25;
	const char RIGHT = 0x27;
}

class Game
{
public:
	Game(std::mutex& sem, Board& board);
	~Game();

	void SetGraphicsEngine(GraphicsEnginePtr graphicsEnginePtr);
	void SetInputHandler(InputHandlerPtr inputHandlerPtr);

	void StartInputHandling(SOCKET socket);

	void DrawMap();
	void MainLoop();
private:
	void Update();
	void RedrawPartOfMap();
	void GetDifferences();

	InputHandlerPtr inputHandlerPtr;
	GraphicsEnginePtr graphicsEnginePtr;

	std::thread inputHandlingThread;
	std::queue<Point> fieldsToRedraw;
	int** map;

	Board& board;
	std::mutex& sem;
};

#endif // !_GAME_H_