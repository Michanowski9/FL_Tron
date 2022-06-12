#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "../structs.h"
#define MAX_MESSAGE_SIZE 80

#define KEY_UP 0x26
#define KEY_DOWN 0x28
#define KEY_LEFT 0x25
#define KEY_RIGHT 0x27

//watek sluchajcy musi wiedziec, ktorego gracza dotyczy (index zawart w zmiennej player) //done
void* startListening(void* arg){
	
	//POBIERANIE DANYCH Z ARGUMENTU
	Player* player = ((Argument*)arg)->player;
	int listenSocket = ((Argument*)arg)->socketOutput;
	pthread_mutex_t* sem = ((Argument*)arg)->sem;
	Board* board = ((Argument*)arg)->board;
	free((Argument*) arg);
	
	bool socketConnected = true;


	char buffer[MAX_MESSAGE_SIZE];
	char key;//WCISNIETY KLAWISZ
	while(socketConnected){ //while socket otwarty
		memset(buffer,0,MAX_MESSAGE_SIZE);
		if(recv(listenSocket,buffer,80,0) == 0){//jesli nic nie odebrano to znaczy ze zerwano polaczenie
			socketConnected = false;
		}		
		printf("server listening socket received: %s\n",buffer);	
		
		if(strncmp(buffer,"INPUT",5) == 0){
			key=buffer[6];
			pthread_mutex_lock(sem);//LOCK SEM
			switch(key){//ROZPATRZENIE KLAWISZA OD KLIENTA
		       		case KEY_UP:
					if(player->lastDirection != DOWN){
						printf("hit w!\n");
						player->direction = UP;
					}
				 	break;
		       		case KEY_DOWN: 
					if(player->lastDirection != UP){
						printf("hit s!\n");
						player->direction = DOWN;
					}
					break;
		       		case KEY_LEFT: 
					if(player->lastDirection != RIGHT){
						printf("hit a!\n");
						player->direction = LEFT;
					}
					break;
		       		case KEY_RIGHT: 
					if(player->lastDirection != LEFT){
						printf("hit d!\n");
						player->direction = RIGHT;
					}
					break;
				case(' '): 
					printf("hit  !\n");
					break;
			}
			pthread_mutex_unlock(sem); //UNLOCK SEM
			
		}				
	}
	
	if(board->runningGame == false){ //czyszczenie gracza w lobby
		Player* temp;
		free(player);
		pthread_mutex_lock(sem);//LOCK SEM
			//swapujemy od lewej do konca
			for(int i=player->index; i< board->maxPlayersNumber - 2;  i++){
				//swap
				temp = board->players[i];
				board->players[i] = board->players[i+1];
				board->players[i+1] = temp;
			}
			board->playersNumber--; 
			if(board->playersNumber < 0){
				board->playersNumber =0;
			}
			printf("cleaned player\n");
		pthread_mutex_unlock(sem);//UNLOCK SEM	

	}

	printf("server listening socket close\n");
	close(listenSocket);
	return NULL;
}


//wysyla poczatkowy stan stolu
//w ten sposob: X I Y to koordynaty poczatkowe kolejnych graczy
//BOARD SIZE PLAYERS_NUMBER PLAYER_INDEX X1 Y1  X2 Y2 ... XN YN
//
//args:
//socketInput - socket przez ktory wyslac
//boardSize - rozmiar planszy
//initPosition - tablcia poczatkowych pozycji o dlugosci playersNumber
void sendInitialBoard(int socketInput,int playerIndex, int boardSize, Position* initPositions, int playersNumber){
	
	char buffer[MAX_MESSAGE_SIZE];
	memset(buffer,0,MAX_MESSAGE_SIZE);
	strcpy(buffer,"BOARD ");

	char numb[10];
	memset(numb,0,10);//pusty string
	sprintf(numb,"%d",boardSize); //size
	strcat(buffer,numb);		
	strcat(buffer," ");

	memset(numb,0,10);//pusty string
	sprintf(numb,"%d",playersNumber); //playersNumber
	strcat(buffer,numb);		
	strcat(buffer," ");

	memset(numb,0,10);//pusty string
	sprintf(numb,"%d",playerIndex); //playerIndex =>kolor
	strcat(buffer,numb);		
	strcat(buffer," ");


	//dodanie init positions
	for(int i=0;i<playersNumber;i++){
		memset(numb,0,10);//pusty string
		sprintf(numb,"%d",initPositions[i].x); //x pos
		strcat(buffer,numb);		
		strcat(buffer," ");

		memset(numb,0,10);//pusty string
		sprintf(numb,"%d",initPositions[i].y); //y pos
		strcat(buffer,numb);		
		strcat(buffer," ");

	}
	printf("buffer: %s\n",buffer);

	send(socketInput,buffer,strlen(buffer),0);
}

///sygnal ze gra sie rozpoczela //DONE
//socketInput - przez jaki socket wyslac
void startGame(int socketInput){
	char buffer[MAX_MESSAGE_SIZE];
	memset(buffer,0,MAX_MESSAGE_SIZE);//zerowanie stringa
	strcpy(buffer,"START_GAME ");
	send(socketInput,buffer,strlen(buffer),0);
}


//wysyla sygnal konca gry wraz informacja czy 
//gracz wygral
//1-wygral
//0-przegral
void endGame(int socketInput, bool alive){
	char buffer[MAX_MESSAGE_SIZE];
	memset(buffer,0,MAX_MESSAGE_SIZE);//zerowanie stringa
	if(alive){
		strcpy(buffer,"END_GAME 1 ");
	}
	else{
		strcpy(buffer,"END_GAME 0 ");
	}
	send(socketInput,buffer,strlen(buffer),0);
}

//wysylka zmian na planszy 
//DIFF N X1 Y1 VAL1 ... XN YN VALN
//difference - tablica zmian typu Difference o dlugosci n
void sendDifference(int socketInput, Difference* difference, int n){
	char buffer[MAX_MESSAGE_SIZE];
	memset(buffer,0,MAX_MESSAGE_SIZE);
	char numb[3];		

	strcpy(buffer,"DIFF ");
	//dodanie liczby roznic
	{
		sprintf(numb,"%d",n); //atoi nie chcialo dzialac
		strcat(buffer,numb);		
		strcat(buffer," ");
	}

	//dodanie krotek roznic
	for(int i=0;i<n;i++){
		sprintf(numb,"%d",difference[i].x); //X
		strcat(buffer,numb);		
		strcat(buffer," ");		

		sprintf(numb,"%d",difference[i].y); //Y
		strcat(buffer,numb);		
		strcat(buffer," ");
	
		sprintf(numb,"%d",difference[i].val); //VAL
		strcat(buffer,numb);		
		strcat(buffer," ");
	}

	printf("%s\n",buffer);

	send(socketInput,buffer,strlen(buffer),0);
}


