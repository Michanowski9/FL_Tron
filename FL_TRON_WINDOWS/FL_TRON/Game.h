#ifndef _GAME_H_
#define _GAME_H_

 
#include "InputHandler.h"
#include "GraphicsEngine.h"

#include <queue>
#include <thread>
#include "Point.h"
#include <mutex>
#include <chrono>

#include "connect.h"

namespace KEY {
	const char UP = 0x26;
	const char DOWN = 0x28;
	const char LEFT = 0x25;
	const char RIGHT = 0x27;
}

class Game
{
public:
	Game() = default;
	~Game();

	void SetMapSize(int size);

	void SetGraphicsEngine(GraphicsEnginePtr graphicsEnginePtr);
	void SetInputHandler(InputHandlerPtr inputHandlerPtr);

	void StartInputHandling(SOCKET socket);
	
	void AddToRedraw(Point point, int value);

	void PrintStartGame();

	void PrintPlayer(int color);

	void DrawMap();
	void MainLoop();
private:
	void AllocateMap();
	void WriteStart(int color);
	void Update();
	void RedrawPartOfMap();

	InputHandlerPtr inputHandlerPtr;
	GraphicsEnginePtr graphicsEnginePtr;

	std::thread inputHandlingThread;
	std::queue<Point> fieldsToRedraw;

	std::mutex sem;

	int** map;
	Point mapSize;
	std::chrono::steady_clock::time_point timer;
	bool text_cleared = false;
};

#endif // !_GAME_H_