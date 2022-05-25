#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "../structs.h"
#define MAX_MESSAGE_SIZE 80

//watek sluchajcy musi wiedziec, ktorego gracza dotyczy (index zawart w zmiennej player) //done
void* startListening(void* arg){
	
	//POBIERANIE DANYCH Z ARGUMENTU
	Player* player = ((Argument*)arg)->player;
	int listenSocket = ((Argument*)arg)->socketOutput;
	pthread_mutex_t* sem = ((Argument*)arg)->sem;
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
		       		case('w'): 
					printf("hit w!\n");
				 	break;
		       		case('s'): 
					printf("hit s!\n");
					break;
		       		case('a'): 
					printf("hit a!\n");
					break;
		       		case('d'): 
					printf("hit d!\n");
					break;
				case(' '): 
					printf("hit  !\n");
					break;
			}
			pthread_mutex_unlock(sem); //UNLOCK SEM
			
		}				
	}
	printf("server listening socket close\n");
	close(listenSocket);
	return NULL;
}


//wysyla poczatkowy stan stolu
//w ten sposob: X I Y to koordynaty poczatkowe kolejnych graczy
//BOARD  X1 Y1  X2 Y2 ... XN YN
//
//args:
//socketInput - socket przez ktory wyslac
//boardSize - rozmiar planszy
//initPosition - tablcia poczatkowych pozycji o dlugosci playersNumber
void sendInitialBoard(int socketInput, int boardSize, Position* initPositions, int playersNumber){
	
	char buffer[MAX_MESSAGE_SIZE];
	memset(buffer,0,MAX_MESSAGE_SIZE);
	strcpy(buffer,"BOARD ");

	//dodanie init positions
	for(int i=0;i<playersNumber;i++){
		char numb[10];
		memset(numb,0,10);//pusty string
		sprintf(numb,"%d",initPositions[i].x); //x pos
		strcat(buffer,numb);		
		strcat(buffer," ");

		memset(numb,0,10);//pusty string
		sprintf(numb,"%d",initPositions[i].y); //y pos
		strcat(buffer,numb);		
		strcat(buffer," ");

	}
	

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

	send(socketInput,buffer,strlen(buffer),0);
}

