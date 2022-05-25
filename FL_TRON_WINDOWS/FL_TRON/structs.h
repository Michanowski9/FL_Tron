#include <mutex>
#include <winsock2.h>
#include <winsock.h>
#include <windows.h>
typedef enum PlayerState{
	DEAD,
	ALIVE,
	BOOSTING
}PlayerState;

typedef enum Direction{
	UP,
	DOWN,
	LEFT,
	RIGHT
}Direction;

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
	PlayerState state;	
	Direction direction;
	int index; //in table of players!
}Player;

typedef struct Board{
	int** tile;
	int size;
	std::mutex* sem;
	Player* players;
	int playersNumber;
	int maxPlayersNumber;	
	PlayerState state; //reuse state'a
}Board;



typedef struct Argument{
        int socketOutput;
        Board* board;      
		std::mutex* sem;
        Player* player;
        int* startGame; //boole tak naprawde
        int* startCounter;
}Argument;

