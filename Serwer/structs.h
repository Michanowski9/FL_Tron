#ifndef _STRUCTS_H_
#define _STRUCTS_H_
#include <stdbool.h>
typedef struct Position{
	int x;
	int y;
}Position;

typedef struct Difference{
	int x;
	int y;
	int val;
}Difference;

typedef enum Direction{
	NONE_DIRECTION,
	UP,
	DOWN,
	LEFT,
	RIGHT
} Direction;

typedef struct Player{
	char* nick; //max size 10
	Position pos;
	int index; //in table of players!
	int value;
	int socket;
	Direction direction;
	Direction lastDirection;
	bool alive;
}Player;

typedef struct Board{
	int** tile;
	int size;
	Player** players; //TABLICA WSKAZNIKOW DO GRACZY
	int playersNumber;
	int maxPlayersNumber;
	pthread_mutex_t* sem;
	bool runningGame;
}Board;



typedef struct Argument{
        int socketOutput;
        Board* board;
        pthread_mutex_t* sem;
        Player* player;
}Argument;


enum PlayerState{
	ALIVE,
	DEAD
};

#endif 
