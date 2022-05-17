#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <math.h>

#include "connect.h"
#include "../stringFuncs.h"

#define MAX_MESSAGE_SIZE 80
#define HTONS 3001
//#define INET_ADDR "127.0.0.1"
//\/ to adres ip mojego kompa \/
#define INET_ADDR "192.168.0.13"     
#define MAX_NICK_LENGTH 10


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
int join(int socketInput, char* nick){
        char buffer[MAX_MESSAGE_SIZE];
        strcpy(buffer,"join ");
	
	strcat(buffer,nick);
	strcat(buffer," ");
        send(socketInput,buffer,strlen(buffer),0);

	memset(buffer,0,MAX_MESSAGE_SIZE);//czyszczenie bufora
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
int createRoom(int socketInput,char* nick,int playersNumber){
        char buffer[MAX_MESSAGE_SIZE];
        strcpy(buffer,"create room ");


	//dodanie nicku:
	strcat(buffer,nick);
	strcat(buffer," ");
	//dodanie liczby graczy
	{
                char numb[3];
        	sprintf(numb,"%d",playersNumber); //atoi nie chcialo dzialac
                strcat(buffer,numb);
		strcat(buffer," ");
        }
	

        send(socketInput,buffer,strlen(buffer),0);
        memset(buffer,0,MAX_MESSAGE_SIZE);//czyszczenie bufora
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



void* turnOnReceiveSocket(void* arg){

	Board* board = ((Argument*)arg)->board;
	int socketOutput = ((Argument*)arg)->socketOutput;
	pthread_mutex_t *sem = ((Argument*)arg)->sem;
	Player* player = ((Argument*)arg)->player;
	int* startCounter;
	int* startGame;
	startCounter=((Argument*)arg)->startCounter;
	startGame=((Argument*)arg)->startGame;

	free((Argument*)arg);

	char buffer[MAX_MESSAGE_SIZE];
	while(1){//<- otwarty(socket)
		memset(buffer,0,MAX_MESSAGE_SIZE);//czyszczenie bufora
        	recv(socketOutput,buffer,MAX_MESSAGE_SIZE,0);
	        printf("cl_rec_sock: %s\n",buffer);		

		//sygnal smierci
		if(strncmp(buffer,"DEATHS",5) == 0){
			int N=buffer[7];
			int bufIndex=8;			
			pthread_mutex_lock(sem);//LOCK
			for(int i=0;i<N;i++){
				if(buffer[i] !=' '){
					int ind=buffer[bufIndex]-'0';
					board->players[ind].state = DEAD;
				}
				bufIndex++;
			}
			pthread_mutex_unlock(sem);//UNLOCK
		}//sygnal zmiany planszy		
		else if(strncmp(buffer,"DIFF",4) == 0){
			int N=buffer[5]-'0';
			int bufIndex=6;						
			pthread_mutex_lock(sem);//LOCK
			for(int i=0;i<N ;i++){
				if(buffer[i] !=' '){
					int x,y,val;
					x=atoi(&buffer[bufIndex]);
					
					printf("buf=%d\n",bufIndex);

					bufIndex=getNextSpaceBar(buffer,bufIndex);
					y=atoi(&buffer[bufIndex]);

					bufIndex=getNextSpaceBar(buffer,bufIndex);
					val=atoi(&buffer[bufIndex]);
					
					
					printf("x=%d,y=%d,val=%d\n",x,y,val);
					board->tile[x][y]=val;
				}
				bufIndex++;
			}
			pthread_mutex_unlock(sem);//UNLOCK							
				
		}else if(strncmp(buffer,"PLAYERS",7) == 0){
			int N=buffer[8]-'0'; //zal ze liczba graczy jest jednocyfrowa
			int bufIndex = 10;			
			Player* players;
			players = (Player*)malloc(sizeof(Player)*N);
			int newPlayerIndex=0;

			for(int i=0; i<N; i++){ //zczytujemy nicki
				char *nick=(char*)malloc(sizeof(char)*10);
				bufIndex = getNextNick(buffer,nick,bufIndex);
				players[i].nick = nick;				

				if(strcmp(nick,player->nick) == 0){//jesli okazuje sie ze zly playerIndex to musi byc tutaj blad wtedy!
					newPlayerIndex = i;
				}
			}


			
			
			pthread_mutex_lock(sem);//LOCK

			free(board->players);
			board->players = players;
			board->playersNumber = N;
			player->index = newPlayerIndex;

			pthread_mutex_unlock(sem);//UNLOCK
			

		}else if(strncmp(buffer,"TABLE",5) == 0){
			int tableSize= atoi(&buffer[6]);
			board->tile = (int**)malloc(sizeof(int*)*tableSize);
                        for(int i=0;i<8;i++){
                                board->tile[i] = (int*)malloc(sizeof(int)*tableSize);
                                for(int j=0;j<tableSize;j++){
                                        board->tile[i][j]=0;
                                }
                        }
                        board->size=tableSize;

		}else if(strncmp(buffer,"START_COUNTER",13) == 0){
			pthread_mutex_lock(sem);//LOCK
			*startCounter = 1;	
			pthread_mutex_unlock(sem);//UNLOCK
		}else if(strncmp(buffer,"START_GAME", 10) == 0){
			pthread_mutex_lock(sem);//LOCK
			*startGame = 1;
			pthread_mutex_unlock(sem);//UNLOCK
		}
	}
}


void CreateReceiveSocket(int socket, Board* board,Player* player,int*startGame, int*startCounter, pthread_mutex_t* sem){
	pthread_t thread_id;

	Argument*arg = (Argument*)malloc(sizeof(Argument));
	arg->board=board;
	arg->sem=sem;
	arg->socketOutput=socket;
	arg->player=player;
	arg->startGame=startGame;
	arg->startCounter=startCounter;
	pthread_create(&thread_id, NULL, &turnOnReceiveSocket, (void*)arg);
}


