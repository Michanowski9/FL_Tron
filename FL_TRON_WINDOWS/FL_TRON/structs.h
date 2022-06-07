#ifndef _STRUCTS_H_
#define _STRUCTS_H_

#include <mutex>
#include <winsock2.h>
#include <winsock.h>
#include <windows.h>

struct Position{
	int x;
	int y;
};

struct Difference{
	int x;
	int y;
	int val;
};

struct Player{
	char* nick; //max size 10
	Position pos;
	int index; //in table of players!
};

struct Board{
	int** tile;
	int size;
	std::mutex* sem;
	Player** players;
	int playersNumber;
	int maxPlayersNumber;	
};

struct Argument {
	SOCKET socketOutput;
	Board* board;
	std::mutex* sem;
	Player* player;
	bool* gameStarted;
};

#endif // !_STRUCTS_H_