#ifndef _CONNECT_H_
#define _CONNECT_H_

#include "structs.h"
#include "stringFuncs.h"

#include "Game.h"
#include <memory>

class Game;

enum MenuDecision{
        JOIN,
        CREATE_ROOM
};

enum{
        ALL_FINE, //bo to zero xd
        JOIN_ERROR,
	    FREE_TABLE_NOT_FOUND,
        CREATE_ROOM_ERROR
};

int connectToServer();

int join(int socketInput);


DWORD WINAPI turnOnReceiveSocket(void* arg);

void CreateReceiveSocket(SOCKET socket, bool* gameStarted, std::mutex* sem, Game* game);

void sendInput(SOCKET socketInput, char key);


#endif // !_CONNECT_H_