#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include "connect.h"

#define MAX_NICK_LENGTH 10

int main (int argc, char *argv[]){
	printf("client: turned on!\n");
	int result;
	int socketInput;
	socketInput = connectToServer();

	//utworzenie zmiennych gry
	int startGame = 0;
	int startCounter = 0;


	//utworzenie gracza
	Player player;
	player.nick = (char*)malloc(sizeof(char) * MAX_NICK_LENGTH);
	strcpy(player.nick,"kuba");
	

	if(socketInput != SO_ERROR){
		printf("client: CONNECTED!\n");
		//tutaj petla sesji

		int playersNumber=2;
		MenuDecision decision=CREATE_ROOM;
		switch(decision){
			case(JOIN): result=join(socketInput);break;
			case(CREATE_ROOM): result=createRoom(socketInput,playersNumber);break;
		}			

		if(result == ALL_FINE){
			//waiting in lobby
			//od tego momentu musi być nowy proces!
			Board board;			
			board.playersNumber=0;
			CreateReceiveSocket(socketInput,&board,&player,&startGame,&startCounter,0);
			

			//wait for game to start 
			//
			//play
			//
			//leave to menu
			sendInput(socketInput,'w');

			//testy lobby
			


		}
		else{
			//sprawdz jaki blad
			printf("some error\n");
		}

	}
	else{
		printf("client: Unable to connect\n");
	}

	close(socketInput);
	printf("client: client killed\n");
	return 0;


}



