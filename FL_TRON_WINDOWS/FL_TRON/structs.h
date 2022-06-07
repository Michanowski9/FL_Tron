#include <mutex>
#include <winsock2.h>
#include <winsock.h>
#include <windows.h>

typedef struct Position{
	int x;
	int y;
}Position;

typedef struct Difference{
	int x;
	int y;
	int val;
}Difference;

typedef struct Player{
	char* nick; //max size 10
	Position pos;
	int index; //in table of players!
}Player;

typedef struct Board{
	int** tile;
	int size;
	std::mutex* sem;
	Player** players;
	int playersNumber;
	int maxPlayersNumber;	
}Board;



typedef struct Argument{
	SOCKET socketOutput;
    Board* board;      
	std::mutex* sem;
    Player* player;
    bool* gameStarted; 
        
}Argument;


