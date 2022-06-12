#include "../structs.h"
#include "../stringFuncs.h"

void* startListening(void* arg);

void sendInitialBoard(int socketInput,int playerIndex, int boardSize, Position* initPositions, int playersNumber);

void startGame(int socketInput);
void endGame(int socketInput,bool alive);

void sendDifference(int socketInput, Difference* difference, int n);

//differnce to cos takiego: [(x1,y1,wartosc_pola),(x2,y2,wartosc_pola),...]
//n-jak duzo roznic (bo zalezy od liczby zyjacych graczy
