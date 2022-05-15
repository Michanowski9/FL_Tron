
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
	Player* players;
	int playersNumber;
	int maxPlayersNumber;
	PlayerState state; //reuse state'a
}Board;
