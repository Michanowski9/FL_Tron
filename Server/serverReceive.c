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

	Player* player = ((Argument*)arg)->player;
	int listenSocket = ((Argument*)arg)->socketOutput;
	pthread_mutex_t* sem = ((Argument*)arg)->sem;
	free((Argument*) arg);
	

	char buffer[MAX_MESSAGE_SIZE];
	char key;
	for(int i=0;i<3;i++){ //while socket otwarty
		memset(buffer,0,MAX_MESSAGE_SIZE);
		recv(listenSocket,buffer,80,0);		
		printf("server listening socket received: %s\n",buffer);	
		
		if(strncmp(buffer,"INPUT",5) == 0){
			key=buffer[6];
			pthread_mutex_lock(sem);//LOCK SEM
			switch(key){
		       		case('w'): printf("hit w!\n");player->direction = UP;break;
		       		case('s'): printf("hit s!\n");player->direction = DOWN;break;
		       		case('a'): printf("hit a!\n");player->direction = LEFT;break;
		       		case('d'): printf("hit d!\n");player->direction = RIGHT;break;
				case(' '): printf("hit  !\n");player->state = BOOSTING;break;
			}
			pthread_mutex_unlock(sem); //UNLOCK SEM
			
		}				
	}
	printf("server listening socket close\n");
	close(listenSocket);
	return NULL;
}


//wysyla aktualny stan lobby do konkretnego gracza //DONE
//poprawia indeks gracza po stronie klienckiej
void updateWaitingRoom(int socketInput, Player* players, int n){ 
	char buffer[MAX_MESSAGE_SIZE];
	memset(buffer,0,MAX_MESSAGE_SIZE);
	strcpy(buffer,"PLAYERS ");//inicjalizacja

	//dodanie liczby graczy
	{
		char numb[3];		
		sprintf(numb,"%d",n); //atoi nie chcialo dzialac
		strcat(buffer,numb);		
	}

	//dodanie nickow graczy
	for(int i=0;i<n;i++){
		strcat(buffer," ");
		strcat(buffer,players[i].nick);

	}
	strcat(buffer," ");

	send(socketInput,buffer,strlen(buffer),0);

}

//wysyla poczatkowa wielkosc tablicy //DONE
void sendInitialBoard(int socketInput, int boardSize){
	
	char buffer[MAX_MESSAGE_SIZE];
	memset(buffer,0,MAX_MESSAGE_SIZE);
	strcpy(buffer,"TABLE ");//inicjalizacja

	//dodanie rozmiaru planszy
	char numb[10];
	memset(numb,0,10);		
	sprintf(numb,"%d",boardSize); //atoi nie chcialo dzialac
	strcat(buffer,numb);		
	
	strcat(buffer," ");


	send(socketInput,buffer,strlen(buffer),0);
}

//wysyla sygnal do rozpoczecia odliczenia//DONE
void startReadyCounter(int socketInput){
	char buffer[MAX_MESSAGE_SIZE];
	memset(buffer,0,MAX_MESSAGE_SIZE);
	strcpy(buffer,"START_COUNTER ");
	send(socketInput,buffer,strlen(buffer),0);
}

//sygnal ze gra sie rozpoczela //DONE
void startGame(int socketInput){
	char buffer[MAX_MESSAGE_SIZE];
	memset(buffer,0,MAX_MESSAGE_SIZE);
	strcpy(buffer,"STARG_GAME ");
	send(socketInput,buffer,strlen(buffer),0);
}

//wysylka zmian na planszy //DONE
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
		sprintf(numb,"%d",difference[i].x); //atoi nie chcialo dzialac
		strcat(buffer,numb);		
		strcat(buffer," ");		

		sprintf(numb,"%d",difference[i].y); //atoi nie chcialo dzialac
		strcat(buffer,numb);		
		strcat(buffer," ");
	
		sprintf(numb,"%d",difference[i].val); //atoi nie chcialo dzialac
		strcat(buffer,numb);		
		strcat(buffer," ");
	}

	send(socketInput,buffer,strlen(buffer),0);


}


