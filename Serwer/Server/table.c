
#include <stdlib.h>
#include "../structs.h"

#define BOARD_SIZE 8

//tu jest glowna petla stolu
void* TurnBoardOn(void*arg){
        Board* board=((Argument*)arg)->board; //RZUTOWANIE I ODCZYT
        free((Argument*)arg);//MILE zwalnianie pamieci


	//ODTAD LOGIKA STOLU BO TO STOL CZEKA NA GRACZY
	bool waitingForPlayers = true;
	while(waitingForPlayers){
		//SPRAWDZENIE CZY FULL
		pthread_mutex_lock(board->sem); //LOCK SEM
		
		if(board->playersNumber == board->maxPlayersNumber){
			waitingForPlayers = false;
		}

		pthread_mutex_unlock(board->sem); //UNLOCK SEM

	}

	//ZDOBYLISMY GRACZY TO CO ROBIMY?
	//
	
	//przygotowanie init pozycji
	Position* initPositions = (Position*)malloc(sizeof(Position)*board->maxPlayersNumber);
	for(int i=0;i<board->maxPlayersNumber;i++){
		board->players[i]->pos.x=6;
		board->players[i]->pos.y=i;
		initPositions[i].x=6;
		initPositions[i].y=i;
	}

	//WYSYLAMY INFO O STOLE (POZYCJE) do kazdego gracza
	for(int i=0;i<board->maxPlayersNumber;i++){
		sendInitialBoard(board->players[i]->socket,board->size,initPositions,board->maxPlayersNumber);
	}

	sleep(1);

	//WYSYLAMY SYGNAL STARTU GRY
	for(int i=0;i<board->maxPlayersNumber;i++){
		startGame(board->players[i]->socket);
	}

	//A TU PETLA GRY
	int liczba=1;
	Difference diff; //single diff, ale normalnie to tablica tam powinna byc
	diff.x=0;
	diff.y=0;
	diff.val=1;
	for(;;){
		for(int i=0;i<board->maxPlayersNumber;i++){
			sendDifference(board->players[i]->socket,&diff,1);
		}
		sleep(1);

		diff.x++;
		if(diff.x >= BOARD_SIZE){
			diff.x=0;
			diff.y++;
			if(diff.y >= BOARD_SIZE){
				diff.y=0;
				diff.val++;
			}

		}
	}


	//DOTAK LOGIKA STOLU
	

	return 0;
}      
