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

#include "serverReceive.h"


#define MAX_NICK_LENGTH 10
#define MAX_MESSAGE_SIZE 80
#define MAX_TABLES 10


//mock do testow 
Board* CreateBoardMock(int maxPlayers){
	Board* table= malloc(sizeof(Board));
	table->playersNumber=0;
	table->maxPlayersNumber=maxPlayers;
	table->players = (Player*)malloc(sizeof(Player)*maxPlayers);
	for(int i=0;i<maxPlayers;i++){
		table->players[i].nick=malloc(sizeof(char*)*MAX_NICK_LENGTH);
	}

	table->size=8;
	table->tile = (int**)malloc(sizeof(int*)*8);
	for(int i=0;i<8;i++){
		table->tile[i] = (int*)malloc(sizeof(int)*8);
		for(int j=0;j<8;j++){
			table->tile[i][j]=0;
		}
	}
	

	return table;
}


void Join(int socketInput, int*tablesNumber, Board* tables[MAX_TABLES]){
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
		memset(buffer,0,strlen(buffer));//czyszczenie bufora
		strcpy(buffer,"OK JOIN");
		send(socketInput,buffer,strlen(buffer),0);		
		//mkthread czy jak to tam bylo
		//tables[tableIndex].AddPlayer(socketInput, Player); <- to powinno byc		
	}
	else{
		memset(buffer,0,strlen(buffer));//czyszczenie bufora
		strcpy(buffer,"NO FREE TABLE");
		send(socketInput,buffer,strlen(buffer),0);
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


void CreateTable(int socketInput, int*tablesNumber, Board** tables, int size){
//tworzenie stolu i dodanie tam gracza
	char buffer[MAX_MESSAGE_SIZE];

	cleanTables(tables,tablesNumber);	


	if(*tablesNumber < MAX_TABLES){
		tables[*tablesNumber] = CreateBoardMock(size);//utworzenie stolu ta funkcja ma tworzyc nowy proces!
		memset(buffer,0,strlen(buffer));//czyszczenie bufora
		strcpy(buffer,"OK CREATE");
		send(socketInput,buffer,strlen(buffer),0);
		tablesNumber += 1;

		//utworzenie procesu sluchajacego i danie go stolowi. tu server konczy swoja robote
		if(fork()==0){ //mkthread
			//dziecko czyli sluchacz
			startListening(socketInput);
		}
		else{
			//dalej server
			//tables[tablesNumber].AddPlayer(socketInput, Player); <- to powinno byc
			//stol po odebraniu nowego gracza rozsyla nowy stan do wszystkich

		}

	}
	else{
		memset(buffer,0,strlen(buffer));//czyszczenie bufora
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

		//czekanie na polaczenie
        	printf("server: waiting for connection\n");
	        listen(listenfd, 10);
		connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);
		printf("server: Somebody connected\n");


		//pierwsza wiadomosc
		memset(buffer,0,strlen(buffer));//czyszczenie bufora
		recv(connfd,buffer,80,0);
		printf("server: received: %s\n",buffer);
		
		//odpowiedz
		if(strcmp(buffer,"join") == 0){
			Join(connfd,&tablesNumber,tables);
		}
		else if(strncmp(buffer,"create room",11) == 0){
			int size = atoi(&buffer[12]);
			CreateTable(connfd,&tablesNumber,tables,size);
		}
				

	}

	close(listenfd);
	printf("server: server killed\n");

        return 0;
}

