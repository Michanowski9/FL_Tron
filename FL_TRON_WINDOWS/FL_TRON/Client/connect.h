#include "../structs.h"
#include "../stringFuncs.h"

typedef enum MenuDecision{
        JOIN,
        CREATE_ROOM
}MenuDecision;

typedef enum{
        ALL_FINE, //bo to zero xd
        JOIN_ERROR,
	    FREE_TABLE_NOT_FOUND,
        CREATE_ROOM_ERROR
}Error;

int connectToServer();

int join(int socketInput);


DWORD WINAPI turnOnReceiveSocket(void* arg);

void CreateReceiveSocket(SOCKET socket, Board* board, Player* player, bool* gameStarted, std::mutex* sem);

void sendInput(SOCKET socketInput, char key);

