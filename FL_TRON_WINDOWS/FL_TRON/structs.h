#ifndef _STRUCTS_H_
#define _STRUCTS_H_

#include <mutex>
#include <winsock2.h>
#include <winsock.h>
#include <windows.h>

#include <memory>
#include "Game.h"

class Game;

struct Argument {
	SOCKET socketOutput;
	bool* gameStarted;
	Game* game;
};

#endif // !_STRUCTS_H_