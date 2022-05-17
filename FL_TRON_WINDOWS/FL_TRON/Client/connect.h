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

int join(int socketInput,char* nick);

int createRoom(int socketInput,char* nick, int playersNumber);


// sem-semaphor
DWORD WINAPI turnOnReceiveSocket(void* arg);

void CreateReceiveSocket(int socket, Board* board, Player* player, int* startGame, int* startCounter, std::mutex* sem);

void sendInput(int socketInput, char key);

