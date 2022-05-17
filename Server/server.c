#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include<errno.h>

#include "table.h"

#define HTONS 3001

#define MAX_NICK_LENGTH 10
#define MAX_MESSAGE_SIZE 80
#define MAX_TABLES 10
#define TABLE_SIZE 100


//utworzenie watku stolu
Board* CreateBoard(int maxPlayers){
	Board* table= malloc(sizeof(Board));
	table->playersNumber=0;
	table->maxPlayersNumber=maxPlayers;
	table->players = (Player*)malloc(sizeof(Player)*maxPlayers);
	table->sem = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));

	pthread_mutex_init(table->sem,0);//inicjalizacja semafora

	for(int i=0;i<maxPlayers;i++){
		table->players[i].nick=malloc(sizeof(char*)*MAX_NICK_LENGTH);
	}

	table->size=TABLE_SIZE;
	table->tile = (int**)malloc(sizeof(int*)*TABLE_SIZE);
	for(int i=0;i<TABLE_SIZE;i++){
		table->tile[i] = (int*)malloc(sizeof(int)*TABLE_SIZE);
		for(int j=0;j<TABLE_SIZE;j++){
			table->tile[i][j]=0;
		}
	}


	//utworzenie nowego watku:
	Argument *arg=(Argument*)malloc(sizeof(Argument));
	arg->board = table;
	pthread_t thread_id;
	pthread_create(&thread_id,NULL,&TurnBoardOn,(void*)arg);
	

	return table;
}


void Join(int socketInput, int*tablesNumber, Board* tables[MAX_TABLES],char*nick){
	//szukanie stolu
	char buffer[MAX_MESSAGE_SIZE];
	int i=0;
	bool found=false;
	int tableIndex=0;
	while(i<*tablesNumber && found == false){
		if(tables[i]->playersNumber < tables[i]->maxPlayersNumber){
			found = true;
			tableIndex=i;
		}
		i++;
	}
	if(found == true){
		memset(buffer,0,MAX_MESSAGE_SIZE);//czyszczenie bufora
		strcpy(buffer,"OK JOIN");
		send(socketInput,buffer,strlen(buffer),0);		
		
		Player *player = (Player*)malloc(sizeof(Player));
		player->nick=(char*)malloc(sizeof(char)*MAX_NICK_LENGTH);
		player->nick = nick;
		player->index = tables[tableIndex]->playersNumber;

		Argument *arg = (Argument*)malloc(sizeof(Argument));
		arg->socketOutput=socketInput;
		arg->player = player;
		arg->sem=tables[tableIndex]->sem; //ten sam semafor co stol

		//dodanie gracza do stolu
		//tables[tableIndex].AddPlayer(socketInput, Player); <- to powinno byc		
		


		//utworzenie watku sluchajacego.
		pthread_t thread_id;
		pthread_create(&thread_id,NULL,&startListening,(void*)arg);
	}
	else{
		memset(buffer,0,MAX_MESSAGE_SIZE);//czyszczenie bufora
		strcpy(buffer,"NO FREE TABLE");
		send(socketInput,buffer,strlen(buffer),0);
		sleep(1);
	}
}


void cleanTables(Board** tables, int* tablesNumber){
	int deletedTables = 0;
	//usuwanie i czyszczenie
	for(int i=0;i<*tablesNumber;i++){
		if(tables[i]->state == DEAD){
			deletedTables++;
			free(tables[i]);
			for(int j=0;i+j<*tablesNumber-1;j++){//przesuwamy
				tables[i+j] = tables[i+j+1];
			}
		}
	}
	
	*tablesNumber-=deletedTables;

}


void CreateTable(int socketInput, int*tablesNumber, Board** tables, int size,char* nick){
//tworzenie stolu i dodanie tam gracza
	char buffer[MAX_MESSAGE_SIZE];

	cleanTables(tables,tablesNumber);	


	if(*tablesNumber < MAX_TABLES){
		Board *table= CreateBoard(size);

		tables[*tablesNumber] = table;
		tablesNumber += 1;
		memset(buffer,0,MAX_MESSAGE_SIZE);//czyszczenie bufora
		strcpy(buffer,"OK CREATE");
		
		send(socketInput,buffer,strlen(buffer),0);
		sleep(1);//bez sleepa glupieje

		Player *player = (Player*)malloc(sizeof(Player));
		player->nick=(char*)malloc(sizeof(char)*MAX_NICK_LENGTH);
		player->nick = nick;
		player->index = table->playersNumber;

		Argument *arg = (Argument*)malloc(sizeof(Argument));
		arg->socketOutput=socketInput;
		arg->player = player;

		//utworzenie watku sluchajacego.
		pthread_t thread_id;
		pthread_create(&thread_id,NULL,&startListening,(void*)arg);
		
		//dalej server
		//tables[tablesNumber].AddPlayer(socketInput, Player); <- to powinno byc,cos tego typu
		//stol po odebraniu nowego gracza rozsyla nowy stan lobby do wszystkich	

	}
	else{
		memset(buffer,0,MAX_MESSAGE_SIZE);//czyszczenie bufora
		strcpy(buffer,"NO ROOM FOR NEW ROOM");
		send(socketInput,buffer,strlen(buffer),0);
	}

}


int main(int argc, char *argv[]){


	printf("serwer start\n");
	
        //przygotowanie
        int listenfd=0,connfd=0;
	int result;
        struct sockaddr_in serv_addr;
	char buffer[80];
	Board **tables=(Board**)malloc(sizeof(Board*)*MAX_TABLES);
	int tablesNumber=0;
	
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


			//pierwsza wiadomosc
			memset(buffer,0,MAX_MESSAGE_SIZE);//czyszczenie bufora
			recv(connfd,buffer,80,0);
			printf("server: received: %s\n",buffer);
			
			//odpowiedz
			if(strncmp(buffer,"join",4) == 0){	
				int bufIndex=5;
				char* nick=(char*)malloc(sizeof(char)*MAX_NICK_LENGTH);
				getNextNick(buffer,nick,bufIndex);
				
				Join(connfd,&tablesNumber,tables,nick);
			}
			else if(strncmp(buffer,"create room",11) == 0){
				int bufIndex=12;
				char* nick=(char*)malloc(sizeof(char)*MAX_NICK_LENGTH);
				bufIndex=getNextNick(buffer,nick,bufIndex);
				int size = atoi(&buffer[12]);
				CreateTable(connfd,&tablesNumber,tables,size,nick);
			}	
		}	

	}

	close(listenfd);
	printf("server: server killed\n");

        return 0;
}

