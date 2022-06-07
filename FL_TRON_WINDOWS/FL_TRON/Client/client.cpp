#define _CRT_SECURE_NO_WARNINGS
#include "connect.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Game.h"
#include "ConsoleOutput.h"
#include "Keyboard.h"
#include "WindowsKeyboard.h"

#define MAX_MESSAGE_SIZE 80
#define TABLE_SIZE 8

using namespace std;


bool GameStarted(mutex* sem, bool* gameStarted);

int main (int argc, char *argv[]){

	auto output = std::make_shared<ConsoleOutput>();

	auto input = std::make_shared<Keyboard>();
	input->SetSystemKeyboard(std::make_shared<WindowsKeyboard>());

	//init socketow
	WORD wersja = MAKEWORD(2, 0);
	WSADATA wsas;
	int result = WSAStartup(wersja, &wsas); //init systemu socketów

	printf("client: turned on!\n");
	
	int socketInput = connectToServer(); //próba połączenia się z serwerem

	if(socketInput != SO_ERROR){
		printf("client: CONNECTED!\n");		
		//DOLACZENIE
		result = join(socketInput);			

		if(result == ALL_FINE){	
			mutex sem;
			bool gameStarted = false;			

			Player player;
			Board board;	
			board.playersNumber=0;
			//tworzenie wątku nasłuchującego
			CreateReceiveSocket(socketInput,&board,&player,&gameStarted,&sem);

			//waiting to start
			while(!GameStarted(&sem,&gameStarted)) {
				Sleep(1);
			}
			printf("game started!\n");

			//setup game
			Game game(sem, board);
			game.SetGraphicsEngine(output);
			game.SetInputHandler(input);

			game.StartInputHandling(socketInput);
			
			game.DrawMap();
			game.MainLoop();
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