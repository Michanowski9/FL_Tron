#include "../structs.h"
#include "../stringFuncs.h"

void* startListening(void* arg);

void sendInitialBoard(int socketInput, int boardSize, Position* initPositions, int playersNumber);

void startGame(int socketInput);

void sendDifference(int socketInput, Difference* difference, int n);

//differnce to cos takiego: [(x1,y1,wartosc_pola),(x2,y2,wartosc_pola),...]
//n-jak duzo roznic (bo zalezy od liczby zyjacych graczy
