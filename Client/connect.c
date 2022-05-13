#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include "connect.h"

#define MAX_MESSAGE_SIZE 80
#define HTONS 3001
#define INET_ADDR "127.0.0.1"

//zwraca socket jesli udalo sie polaczyc z serwerem
//w przeciwnym razie -1
int connectToServer(){
	int socketInput=socket(AF_INET,SOCK_STREAM,0);
        struct sockaddr_in server;
        memset(&server,0,sizeof(server));
        server.sin_family = AF_INET;
        server.sin_port = htons(HTONS);
        server.sin_addr.s_addr = inet_addr(INET_ADDR);
	
	int result;

        result = connect(socketInput, (struct sockaddr*)&server, sizeof(server));
	if(result == -1){
		return SO_ERROR;
	}

	return socketInput;

}


//sygnal wysylany do serwera, ze gracz chce
//dolaczyc do dowolnego stolu
//zwraca 0 gdy sie udalo
//w przeciwnym razie FREE_TABLE_NOT_FOUND
int join(int socketInput){
        char buffer[MAX_MESSAGE_SIZE];
        strcpy(buffer,"join");
        send(socketInput,buffer,strlen(buffer),0);

	memset(buffer,0,strlen(buffer));//czyszczenie bufora
        recv(socketInput,buffer,MAX_MESSAGE_SIZE,0);
        printf("client: server respone: %s\n",buffer);
        if(strcmp(buffer,"OK JOIN") == 0){
                printf("client: joined: \n");
        }
        else{
        	if(strncmp(buffer,"NO FREE TABLE",strlen("NO FREE TABLE")) == 0){
	                printf("client: no free table\n");
                	return FREE_TABLE_NOT_FOUND;
		}
        }


        return 0;

}


//metoda tworzaca stol
//zero gdy sie powiodlo
//CREATE_ROOM_ERROR gdy nie
int createRoom(int socketInput,int playersNumber){
        char buffer[MAX_MESSAGE_SIZE];
        strcpy(buffer,"create room");
        send(socketInput,buffer,strlen(buffer),0);
        memset(buffer,0,strlen(buffer));//czyszczenie bufora
        recv(socketInput,buffer,MAX_MESSAGE_SIZE,0);
        printf("client: server respone: %s\n",buffer);
        if(strcmp(buffer,"OK CREATE") == 0){
                printf("client: created room: \n");
        }
        else{
                printf("client: create room error\n");
                return CREATE_ROOM_ERROR;
        }


        return 0;

}


void sendInput(int socketInput, char key){
        char buffer[MAX_MESSAGE_SIZE];
        strcpy(buffer,"INPUT ");

	strncat(buffer, &key,1);

        send(socketInput,buffer,strlen(buffer),0);
        
}

void turnOnReceiveSocket(int socketOutput, void* board, void* sem){
	char buffer[MAX_MESSAGE_SIZE];
	while(1){//<- otwarty(socket)
		memset(buffer,0,strlen(buffer));//czyszczenie bufora
        	recv(socketOutput,buffer,MAX_MESSAGE_SIZE,0);
	        printf("cl_rec_sock: %s\n",buffer);
		
		

	}
}

