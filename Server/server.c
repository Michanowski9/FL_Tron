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

#define MAX_TABLES 10

typedef struct{
	int playersNumber;
	int maxPlayersNumber;
	char** nicks;//tablica nickow
}TableMock;


TableMock* CreateTable(int maxPlayers){
	TableMock* table= malloc(sizeof(TableMock));
	table->playersNumber=0;
	table->maxPlayersNumber=maxPlayers;
	table->nicks = malloc(sizeof(char*)*maxPlayers);
	for(int i=0;i<maxPlayers;i++){
		table->nicks[i]=malloc(sizeof(char*)*80);
	}
	return table;
}

int main(int argc, char *argv[]){


	printf("serwer start\n");
	
        //przygotowanie
        int listenfd=0,connfd=0;
	int result;
        struct sockaddr_in serv_addr;
	char buffer[80];
	TableMock *tables[MAX_TABLES];
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
			//szukanie stolu
			int i=0;
			bool found=false;
			while(i<tablesNumber && found == false){
				if(tables[i]->playersNumber<tables[i]->maxPlayersNumber){
					found = true;
				}
				i++;
			}

			if(found == true){
				memset(buffer,0,strlen(buffer));//czyszczenie bufora
				strcpy(buffer,"OK JOIN");
				send(connfd,buffer,strlen(buffer),0);
				//a tu wyślij stann lobby
				//jako druga wiadomosc
			}
			else{
				memset(buffer,0,strlen(buffer));//czyszczenie bufora
				strcpy(buffer,"NO FREE TABLE");
				send(connfd,buffer,strlen(buffer),0);
			}
		}
		else if(strcmp(buffer,"create room") == 0){
			//tworzenie stolu i dodanie tam gracza

			if(tablesNumber<MAX_TABLES){
				tables[tablesNumber]=CreateTable(2);
				memset(buffer,0,strlen(buffer));//czyszczenie bufora
				strcpy(buffer,"OK CREATE");
				send(connfd,buffer,strlen(buffer),0);
				tablesNumber += 1;

				//utworzenie procesu sluchajacego i danie go stolowi. tu server konczy swoja robote
				if(fork()==0){
					//dziecko czyli sluchacz
					startListening(connfd);
				}
				else{
					//dalej server
					//tables[tablesNumber].AddPlayer(socket:); <- to powinno byc
				}

			}
			else{
				memset(buffer,0,strlen(buffer));//czyszczenie bufora
				strcpy(buffer,"NO ROOM FOR NEW ROOM");
				send(connfd,buffer,strlen(buffer),0);
			}
		}
				

	}

	close(listenfd);
	printf("server: server killed\n");

        return 0;
}

