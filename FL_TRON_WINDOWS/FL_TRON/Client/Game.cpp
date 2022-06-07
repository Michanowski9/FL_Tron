#include "Game.h"

Game::Game(std::mutex& sem, Board& board) : sem(sem), board(board)
{
	map = new int* [board.size];
	for (int i = 0; i < board.size; i++)
	{
		map[i] = new int[board.size]{};
	}
}

Game::~Game()
{
	inputHandlingThread.join();
	for (int i = 0; i < board.size; i++)
	{
		delete[] map[i];
	}
	delete[] map;
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

void Game::DrawMap()
{
	system("cls");
	for (int row = 0; row < board.size; row++) {
		for (int col = 0; col < board.size; col++) {
			graphicsEnginePtr->DrawCell(row, col, map[row][col]);
		}
	}
}

void Game::MainLoop()
{
	while (true) {
		Update();

		sem.lock();
		RedrawPartOfMap();
		sem.unlock();

		Sleep(1000);
	}
}

void Game::Update()
{
	GetDifferences();
}

void Game::RedrawPartOfMap()
{
	while (fieldsToRedraw.size() > 0)
	{
		auto field = fieldsToRedraw.front();
		fieldsToRedraw.pop();
		graphicsEnginePtr->DrawCell(field.x, field.y, map[field.x][field.y]);
	}
}

void Game::GetDifferences()
{
	for (int x = 0; x < board.size; x++)
	{
		for (int y = 0; y < board.size; y++)
		{
			if (board.tile[x][y] != map[x][y]) {
				map[x][y] = board.tile[x][y];
				fieldsToRedraw.push(Point(x, y));
			}
		}
	}
}
