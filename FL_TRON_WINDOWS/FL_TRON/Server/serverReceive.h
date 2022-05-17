#include "../structs.h"
#include "../stringFuncs.h"

DWORD WINAPI startListening(void* arg);

void updateWaitingRoom(int socketInput, Player* players,int n); //player nazwa,indeks w grze

void sendInitialBoard(int socketInput, int boardSize);

void startReadyCounter(int socketInput);

void startGame(int socketInput);

void sendDifference(int socketInput, Difference* difference, int n);

//differnce to cos takiego: [(x1,y1,wartosc_pola),(x2,y2,wartosc_pola),...]
//n-jak duzo roznic (bo zalezy od liczby zyjacych graczy
