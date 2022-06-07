#include "Game.h"

Game::~Game()
{
	inputHandlingThread.join();
	for (int i = 0; i < mapSize.x; i++)
	{
		delete[] map[i];
	}
	delete[] map;
}

void Game::SetMapSize(int size)
{
	this->mapSize = Point(size, size);

	this->AllocateMap();
}

void Game::AllocateMap()
{
	map = new int* [mapSize.x];
	for (int i = 0; i < mapSize.x; i++)
	{
		map[i] = new int[mapSize.y]{};
	}
}

void Game::SetGraphicsEngine(GraphicsEnginePtr graphicsEnginePtr)
{
	this->graphicsEnginePtr = graphicsEnginePtr;
}

void Game::SetInputHandler(InputHandlerPtr inputHandlerPtr)
{
	this->inputHandlerPtr = inputHandlerPtr;
}

void Game::StartInputHandling(SOCKET socket)
{
	inputHandlingThread = std::thread([](SOCKET socket, InputHandlerPtr input) {
		auto lastKey = ' ';
		while (true) {
			if (input->GetKeyPressed(KEY::UP) && lastKey != KEY::UP && lastKey != KEY::DOWN) {
				lastKey = KEY::UP;
				sendInput(socket, lastKey);
			}
			if (input->GetKeyPressed(KEY::DOWN) && lastKey != KEY::DOWN && lastKey != KEY::UP) {
				lastKey = KEY::DOWN;
				sendInput(socket, lastKey);
			}
			if (input->GetKeyPressed(KEY::LEFT) && lastKey != KEY::LEFT && lastKey != KEY::RIGHT) {
				lastKey = KEY::LEFT;
				sendInput(socket, lastKey);
			}
			if (input->GetKeyPressed(KEY::RIGHT) && lastKey != KEY::RIGHT && lastKey != KEY::LEFT) {
				lastKey = KEY::RIGHT;
				sendInput(socket, lastKey);
			}
		}
	}, socket, inputHandlerPtr);
}

void Game::AddToRedraw(Point point, int value)
{
	map[point.x][point.y] = value;

	sem.lock();
	fieldsToRedraw.push(point);
	sem.unlock();
}

void Game::DrawMap()
{
	system("cls");
	for (int row = 0; row < mapSize.x; row++) {
		for (int col = 0; col < mapSize.y; col++) {
			graphicsEnginePtr->DrawCell(row, col, map[row][col]);
		}
	}
}

void Game::MainLoop()
{
	while (true) {
		Update();
		RedrawPartOfMap();
	}
}

void Game::Update()
{

}

void Game::RedrawPartOfMap()
{
	sem.lock();
	while (fieldsToRedraw.size() > 0)
	{
		Point field = fieldsToRedraw.front();
		fieldsToRedraw.pop();
		graphicsEnginePtr->DrawCell(field.x, field.y, map[field.x][field.y]);
	}
	sem.unlock();
}