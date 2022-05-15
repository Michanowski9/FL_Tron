#include "../structs.h"

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

int createRoom(int socketInput, int playersNumber);


// sem-semaphor
void* turnOnReceiveSocket(void* arg);

void CreateReceiveSocket(int socket, Board* board,Player* player, int* startGame,int*startCounter, void* sem);

void sendInput(int socketInput, char key);

