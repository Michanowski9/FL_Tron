#include <unistd.h>
#include <stdio.h>
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
		board->players[i]->index = i;
		sendInitialBoard(board->players[i]->socket,board->players[i]->index, board->size,
				initPositions,board->maxPlayersNumber);
	}
}

void SendEndGameSignal(Board* board){
	for(int i=0;i<board->maxPlayersNumber;i++){
		endGame(board->players[i]->socket, board->players[i]->alive);
	}
}


void CleanTable(Board* board){
	//zerujemy pola
	for(int i=0; i < board->size; i++){
		for(int j=0; j<board->size; j++){
			board->tile[i][j] = 0;
		}
	}

	//usuwamy graczy
	
	for(int i=0; i<board->maxPlayersNumber; i++){
		free(board->players[i]);
	}
	
	board->playersNumber = 0;
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
					player->lastDirection = UP;
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
					player->lastDirection = DOWN;
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
					player->lastDirection = LEFT;
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
					player->lastDirection = RIGHT;
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


//sprawdza czy gra trwa
//czyli, czy jest wiecej niz jeden gracz zyje
bool gameRunning(Board* board){
	int alivePlayers = 0;
	for (int i=0; i<board->maxPlayersNumber; i++){
		if(board->players[i]->alive){
			alivePlayers++;
		}
	}

	if(alivePlayers > 0){
		return true;
	}
	else{
		return false;
	}
	
}


//tu jest glowna petla stolu
void* TurnBoardOn(void*arg){
        Board* board=((Argument*)arg)->board; //RZUTOWANIE I ODCZYT
        free((Argument*)arg);//MILE zwalnianie pamieci


	//A TU PETLA GRY
	Difference* diffs;
	diffs = (Difference*)malloc(sizeof(Difference)*board->maxPlayersNumber);
	int diffNumber;
	for(;;){ // petla bez konca, dopoki serwer nie zostanie zamkniety

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

		board->runningGame = true;

		//przygotowanie init pozycji
		Position* initPositions  = createInitPositions(board);

		//WYSYLAMY INFO O STOLE (POZYCJE) do kazdego gracza
		SendInitPositions(board,initPositions);
		sleep(4);

		//WYSYLAMY SYGNAL STARTU GRY
		for(int i=0;i<board->maxPlayersNumber;i++){
			startGame(board->players[i]->socket);
		}
		



		for(;gameRunning(board);){ //dopoki liczba zyjacych graczy != 1		
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
			usleep(100000); //calkiem dobre			
		
		}
		//informujemy o zakonczeniu gry
		SendEndGameSignal(board);
		//czyszczenie stolu, gdy gra sie zakonczyla
		CleanTable(board);
		printf("table cleaned\n");
		sleep(2);
		board->runningGame = false;
	}

	printf("table closed\n");

	return 0;
}      
