#include <unistd.h>
#include <stdlib.h>
#include "../structs.h"
#include "serverReceive.h"
#include <pthread.h>






//przygotowanie init pozycji
//i kierunku
Position* createInitPositions(Board* board){
	Position* initPositions = (Position*)malloc(sizeof(Position)*board->maxPlayersNumber);
	for(int i=0;i<board->maxPlayersNumber;i++){
		initPositions[i].x=i*i+3*i + 1;
		initPositions[i].y=2;
		board->players[i]->pos.x=initPositions[i].x;
		board->players[i]->pos.y=initPositions[i].y;
		board->players[i]->value=i+1;
		board->players[i]->alive=true;

		board->players[i]->direction = DOWN;
	}
	return initPositions;
}




void SendInitPositions(Board* board, Position* initPositions){
	for(int i=0;i<board->maxPlayersNumber;i++){
		sendInitialBoard(board->players[i]->socket,board->size,initPositions,board->maxPlayersNumber);
	}
}




	
enum PlayerState MovePlayer(Player* player, Difference* differences, int *diffNumber, Board* board){
	int x=player->pos.x;
	int y=player->pos.y;
	if(player->alive){
		//przemiesc i sprawdz krawedzie czy mozna przemiescic, gdy nie zabij gracza
		// oraz sprawdz czy nastepne pole nie jest zablokowane
		switch(player->direction){
			case(UP): 
				if(y == 0 || board->tile[y-1][x] != 0){
					//umrzyj
					player->alive = false;
				}
				else{					
					player->pos.y -= 1;
					differences[*diffNumber].val = player->value;
					differences[*diffNumber].x = player->pos.x;
					differences[*diffNumber].y = player->pos.y;

					*diffNumber += 1;
				}
				break;

			case(DOWN):
				if(y == board->size - 1 || board->tile[y+1][x] != 0){
					//umrzyj
					player->alive = false;
				}
				else{
					player->pos.y += 1;
					differences[*diffNumber].val = player->value;
					differences[*diffNumber].x = player->pos.x;
					differences[*diffNumber].y = player->pos.y;
					*diffNumber += 1;
				}
				break;

			case(LEFT):
				if(x == 0 || board->tile[y][x-1] != 0){
					//umrzyj
					player->alive = false;
				}
				else{
					player->pos.x -= 1;
					differences[*diffNumber].val = player->value;
					differences[*diffNumber].x = player->pos.x;
					differences[*diffNumber].y = player->pos.y;
					*diffNumber += 1;
				}
				break;

			case(RIGHT):
				if(x == board->size -1  || board->tile[y][x+1] != 0){
					//umrzyj
					player->alive = false;
				}
				else{
					player->pos.x += 1;
					differences[*diffNumber].val = player->value;
					differences[*diffNumber].x = player->pos.x;
					differences[*diffNumber].y = player->pos.y;
					*diffNumber += 1;
				}
				break;
		}
		board->tile[player->pos.y][player->pos.x]=player->value;

	}
	return player->alive;
}


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


	//przygotowanie init pozycji
	Position* initPositions  = createInitPositions(board);

	//WYSYLAMY INFO O STOLE (POZYCJE) do kazdego gracza
	SendInitPositions(board,initPositions);
	sleep(1);

	//WYSYLAMY SYGNAL STARTU GRY
	for(int i=0;i<board->maxPlayersNumber;i++){
		startGame(board->players[i]->socket);
	}

	//A TU PETLA GRY
	Difference* diffs;
	diffs = (Difference*)malloc(sizeof(Difference)*board->maxPlayersNumber);
	int diffNumber;
	for(;;){ //dopoki liczba zyjacych graczy != 1
		
		pthread_mutex_lock(board->sem); //LOCK SEM
		diffNumber = 0;
		for(int i=0; i<board->maxPlayersNumber; i++){
			if(MovePlayer(board->players[i], diffs, &diffNumber,board) == DEAD){
				//dead
			}
		}


		
		for(int i=0;i<board->maxPlayersNumber;i++){
			sendDifference(board->players[i]->socket,diffs,diffNumber);
		}
		
		pthread_mutex_unlock(board->sem); //UNLOCK SEM
		//sleep(1);
		usleep(200000);
	
	}

	return 0;
}      
