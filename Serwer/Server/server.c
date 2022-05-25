#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include<errno.h>

#include "table.h"

#define HTONS 3001

#define MAX_PLAYERS 1
#define MAX_MESSAGE_SIZE 80
#define TABLE_SIZE 8


//utworzenie watku stolu
Board* CreateBoard(int maxPlayers){
	Board* table= malloc(sizeof(Board));//REZERWACJA MIEJSCA NA STOL
	table->playersNumber=0; //PRZY TWORZENIU NIE MA ZADNYCH GRACZY
	table->maxPlayersNumber=maxPlayers; //USTAWIENIE MAX LICZBY GRACZY
	table->players = (Player**)malloc(sizeof(Player*)*maxPlayers); //REZERWACJA MIEJSCA NA WSKAZNIKI NA GRACZY
	table->sem = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));//REZ. MIEJSCA NA SEMAFOR

	pthread_mutex_init(table->sem,0);//inicjalizacja semafora

	table->size=TABLE_SIZE; //PRZYPISANIE ROZMIARU PLANSZY
	table->tile = (int**)malloc(sizeof(int*)*TABLE_SIZE); //REZ. MIEJSCA NA POLA PLANSZY
	for(int i=0;i<TABLE_SIZE;i++){
		table->tile[i] = (int*)malloc(sizeof(int)*TABLE_SIZE); // A TU NA KAŻDY WIERSZ
		for(int j=0;j<TABLE_SIZE;j++){
			table->tile[i][j]=0;   //USTAWIENIE POLA JAKO PUSTEGO
		}
	}


	//utworzenie nowego watku:
	Argument *arg=(Argument*)malloc(sizeof(Argument));
	arg->board = table;
	pthread_t thread_id;
	pthread_create(&thread_id,NULL,&TurnBoardOn,(void*)arg);//UTWORZENIE WATKU STOLU
	

	return table;
}


void Join(int socketInput,Board* board){
	//szukanie stolu
	char buffer[MAX_MESSAGE_SIZE]; //BUFFER NA WIADOMOSCI
	bool found=false;

	if(board->playersNumber <= board->maxPlayersNumber){
		found = true;
	}


	if(found == true){ //BYLO WOLNE MIEJSCE
		memset(buffer,0,MAX_MESSAGE_SIZE);//czyszczenie bufora
		strcpy(buffer,"OK JOIN"); 
		send(socketInput,buffer,strlen(buffer),0);//WYSYLANIE POTWIERDZENIA
		
		//INICJALIZACJA GRACZA
		Player *player = (Player*)malloc(sizeof(Player));
		player->index = board->playersNumber;
		player->socket = socketInput;

		Argument *arg = (Argument*)malloc(sizeof(Argument));
		arg->socketOutput=socketInput;
		arg->player = player;
		arg->sem=board->sem; //ten sam semafor co stol

		//dodanie gracza do stolu
		board->players[board->playersNumber] = player;
		
		pthread_mutex_lock(board->sem);
		board->playersNumber++;
		pthread_mutex_unlock(board->sem);


		//utworzenie watku sluchajacego.
		pthread_t thread_id;
		pthread_create(&thread_id,NULL,&startListening,(void*)arg);
	}
	else{
		//PELNY STOL
		memset(buffer,0,MAX_MESSAGE_SIZE);//czyszczenie bufora
		strcpy(buffer,"NO FREE TABLE");
		send(socketInput,buffer,strlen(buffer),0);
		sleep(1);
	}
}


int main(int argc, char *argv[]){


	printf("serwer start\n");
	
        //przygotowanie
        int listenfd=0,connfd=0;
	int result; //REZULTAT POWODZENIA FUNKCJI KONIECZNYCH DO DZIALANIA 
        struct sockaddr_in serv_addr;
	char buffer[80]; //STRING BUFFER
	Board *board = CreateBoard(MAX_PLAYERS); // UTWORZENIE STOLU, BO TRZEBA JUZ NA POCZATKU
	
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
        memset(&serv_addr, 0, sizeof(serv_addr));

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(3001);
        serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	result = bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
	if(result != 0){
		printf("server: binding error %d %s\n",errno,strerror(errno));
	}
	else{
		for(;;){
			//czekanie na polaczenie
			printf("server: waiting for connection\n");
			listen(listenfd, 10);
			connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);
			printf("server: Somebody connected\n");


			memset(buffer,0,MAX_MESSAGE_SIZE);//czyszczenie bufora
			recv(connfd,buffer,80,0); //pierwsza OTZRYMANA wiadomosc
			printf("server: received: %s\n",buffer);
			
			//CO KLIENT CHCIAL OD SERWERA:
			if(strncmp(buffer,"join",4) == 0){					
				Join(connfd,board);
			}
		}	

	}

	close(listenfd);
	printf("server: server killed\n");

        return 0;
}

