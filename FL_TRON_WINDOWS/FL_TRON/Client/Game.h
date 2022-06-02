#ifndef _GAME_H_
#define _GAME_H_

#include <mutex>
#include "../structs.h"

#include "GraphicsEngine.h"
#include <queue>

struct Point {
	Point(int x, int y) : x(x), y(y) {}
	int x;
	int y;
};

class Game
{
public:
	Game(std::mutex& sem, Board& board);
	~Game();

	void SetGraphicsEngine(GraphicsEnginePtr graphicsEnginePtr);

	void DrawMap();
	void MainLoop();
private:
	void HandleInput();
	void Update();
	void RedrawPartOfMap();
	void GetDifferences();

	GraphicsEnginePtr graphicsEnginePtr;

	std::queue<Point> fieldsToRedraw;
	int** map;
	Board& board;
	std::mutex& sem;
};

#endif // !_GAME_H_