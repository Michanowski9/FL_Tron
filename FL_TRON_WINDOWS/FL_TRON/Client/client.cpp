#define _CRT_SECURE_NO_WARNINGS
#include "connect.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_NICK_LENGTH 10

using namespace std;

int main (int argc, char *argv[]){
	//init socketow
	int result;
	WSADATA wsas;
	WORD wersja;
	wersja = MAKEWORD(2, 0);
	result=WSAStartup(wersja, &wsas);



	printf("client: turned on!\n");
	
	int socketInput;	
	mutex sem;

	//utworzenie zmiennych gry
	int startGame = 0;
	int startCounter = 0;


	//utworzenie gracza
	Player player;
	player.nick = (char*)malloc(sizeof(char) * MAX_NICK_LENGTH);
	strcpy(player.nick,"kuba");
	
	socketInput = connectToServer();

	if(socketInput != SO_ERROR){
		printf("client: CONNECTED!\n");
		//tutaj petla sesji

		int playersNumber=2;
		MenuDecision decision=JOIN;
		switch(decision){
			case(JOIN): result=join(socketInput,player.nick);break;
			case(CREATE_ROOM): result=createRoom(socketInput,player.nick,playersNumber);break;
		}			

		if(result == ALL_FINE){
			//waiting in lobby
			//od tego momentu musi być nowy proces!
			Board board;			
			board.playersNumber=0;
			CreateReceiveSocket(socketInput,&board,&player,&startGame,&startCounter,&sem);
			

			//wait for game to start 
			//
			//play
			//
			//leave to menu
			sendInput(socketInput,'w');

			//notka do michala:
			//semafory sa dla:
			//-pol planszy
			//-stanu gracza
			//-sygnal startu gry
			//-sygnal startu odliczania
			//-gracze w lobby - co do tego to niech klient sprawdza co jakis czas, czy ktos nowy nie wszedl
			


		}
		else{
			//sprawdz jaki blad
			printf("some error\n");
		}

	}
	else{
		printf("client: Unable to connect\n");
	}

	printf("client: client killed\n");
	return 0;


}



