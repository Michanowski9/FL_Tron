#define _CRT_SECURE_NO_WARNINGS
#include "connect.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_MESSAGE_SIZE 80
#define TABLE_SIZE 8

using namespace std;


/// <summary>
/// sprawdza czy gra się już rozpoczęła(czy był sygnał do serwera)
/// zwracal true, gdy gra się już rozpoczęła,
/// w przeciwnym razie false.
/// </summary>
/// <param name="sem">semafor</param>
/// <param name="gameStarted">zmienna gdzie zachowany jest stan czy gra sie rozpoczeła</param>
/// <returns></returns>
bool GameStarted(mutex* sem, bool* gameStarted) {
	bool started;
	sem->lock();//LOCK
	started = *gameStarted;
	sem->unlock();//UNLOCK
	
	return started;
}


int main (int argc, char *argv[]){
	//init socketow
	int result;
	WSADATA wsas;
	WORD wersja;
	wersja = MAKEWORD(2, 0);
	result=WSAStartup(wersja, &wsas); //INIT SYSTEMU SOCKETOW


	printf("client: turned on!\n");
	
	int socketInput;	
	mutex sem;

	//utworzenie zmiennych gry
	bool gameStarted = false;	


	//utworzenie gracza
	Player player;		
	
	//POLĄCZENIE
	socketInput = connectToServer();//PROBA POLACZENIA SIE Z SERWEREM

	if(socketInput != SO_ERROR){
		printf("client: CONNECTED!\n");		
		//DOLACZENIE
		result=join(socketInput);			

		if(result == ALL_FINE){			
			
			Board board;			
			board.playersNumber=0;
			//tworzenie wątku nasłuchującego
			CreateReceiveSocket(socketInput,&board,&player,&gameStarted,&sem);
			
			//waiting to start
			while(!GameStarted(&sem,&gameStarted)) {
				Sleep(1);
			}
			printf("game started!\n");

			for (;;) {//PETLA GRY <- JAK WIDZISZ NIE WARUNKU STOPU
				//DRAW BOARD
				sem.lock();//LOCK
				for (int row = 0; row < board.size; row++) {
					for (int col = 0; col < board.size; col++) {
						printf("%d", board.tile[row][col]);
					}
					printf("\n");

				}
				sem.unlock();//UNLOCK
				printf("\n");
				Sleep(1000);
			}
			


		}
		else{			
			printf("join error\n");
		}

	}
	else{
		printf("client: Unable to connect\n");
	}

	printf("client: client killed\n");
	return 0;


}



