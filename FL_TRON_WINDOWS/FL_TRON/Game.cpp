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

void Game::WriteStart(int color) {
	// S
	graphicsEnginePtr->DrawCell(5, 13, color);
	graphicsEnginePtr->DrawCell(6, 13, color);
	graphicsEnginePtr->DrawCell(7, 13, color);
	graphicsEnginePtr->DrawCell(5, 14, color);
	graphicsEnginePtr->DrawCell(5, 15, color);
	graphicsEnginePtr->DrawCell(6, 15, color);
	graphicsEnginePtr->DrawCell(7, 15, color);
	graphicsEnginePtr->DrawCell(7, 16, color);
	graphicsEnginePtr->DrawCell(7, 17, color);
	graphicsEnginePtr->DrawCell(6, 17, color);
	graphicsEnginePtr->DrawCell(5, 17, color);

	// T
	graphicsEnginePtr->DrawCell(9, 13, color);
	graphicsEnginePtr->DrawCell(10, 13, color);
	graphicsEnginePtr->DrawCell(11, 13, color);
	graphicsEnginePtr->DrawCell(10, 14, color);
	graphicsEnginePtr->DrawCell(10, 15, color);
	graphicsEnginePtr->DrawCell(10, 16, color);
	graphicsEnginePtr->DrawCell(10, 17, color);

	// A
	graphicsEnginePtr->DrawCell(13, 13, color);
	graphicsEnginePtr->DrawCell(13, 14, color);
	graphicsEnginePtr->DrawCell(13, 15, color);
	graphicsEnginePtr->DrawCell(13, 16, color);
	graphicsEnginePtr->DrawCell(13, 17, color);

	graphicsEnginePtr->DrawCell(14, 13, color);
	graphicsEnginePtr->DrawCell(14, 15, color);

	graphicsEnginePtr->DrawCell(15, 13, color);
	graphicsEnginePtr->DrawCell(15, 14, color);
	graphicsEnginePtr->DrawCell(15, 15, color);
	graphicsEnginePtr->DrawCell(15, 16, color);
	graphicsEnginePtr->DrawCell(15, 17, color);

	// R
	graphicsEnginePtr->DrawCell(17, 13, color);
	graphicsEnginePtr->DrawCell(17, 14, color);
	graphicsEnginePtr->DrawCell(17, 15, color);
	graphicsEnginePtr->DrawCell(17, 16, color);
	graphicsEnginePtr->DrawCell(17, 17, color);

	graphicsEnginePtr->DrawCell(18, 13, color);
	graphicsEnginePtr->DrawCell(18, 15, color);
	graphicsEnginePtr->DrawCell(18, 16, color);

	graphicsEnginePtr->DrawCell(19, 13, color);
	graphicsEnginePtr->DrawCell(19, 14, color);
	graphicsEnginePtr->DrawCell(19, 15, color);
	graphicsEnginePtr->DrawCell(19, 17, color);

	// T
	graphicsEnginePtr->DrawCell(21, 13, color);
	graphicsEnginePtr->DrawCell(22, 13, color);
	graphicsEnginePtr->DrawCell(23, 13, color);
	graphicsEnginePtr->DrawCell(22, 14, color);
	graphicsEnginePtr->DrawCell(22, 15, color);
	graphicsEnginePtr->DrawCell(22, 16, color);
	graphicsEnginePtr->DrawCell(22, 17, color);
}

void Game::PrintStartGame()
{
	WriteStart(10);	
}

void Game::PrintPlayer(int color)
{
	graphicsEnginePtr->DrawCell(75, 0, color);
	graphicsEnginePtr->DrawCell(76, 0, color);
	graphicsEnginePtr->DrawCell(77, 0, color);
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
	timer++;
	if (timer == 1000) {
		WriteStart(0);
	}
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