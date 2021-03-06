#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "connect.h"


// if you want to write out to console buf, x, y, val uncomment this constant
//#define TEST_OUTPUT

#define MAX_MESSAGE_SIZE 80
#define HTONS 3001

/// Server IPv4 address, you can find it in your operating system on computer which is running server instance
/// Settings -> Network -> (Settings Icon) -> IPv4 Address
#define INET_ADDR "192.168.214.36"

#define TABLE_SIZE 8
#define MAX_NICK_LENGTH 10
#define MAX_PLAYERS 1

using namespace std;

//zwraca socket jesli udalo sie polaczyc z serwerem
//w przeciwnym razie -1
int connectToServer() {
	SOCKET socketInput = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in server;
	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(HTONS);
	server.sin_addr.s_addr = inet_addr(INET_ADDR);

	int result;

	result = connect(socketInput, (struct sockaddr*)&server, sizeof(server));
	if (result == SOCKET_ERROR) {
		return SO_ERROR;
	}

	return (int)socketInput;

}


//sygnal wysylany do serwera, ze gracz chce
//dolaczyc do dowolnego stolu
//zwraca 0 gdy sie udalo
//w przeciwnym razie FREE_TABLE_NOT_FOUND
int join(int socketInput) {
	char buffer[MAX_MESSAGE_SIZE];
	bool canJoin = false;
	strcpy(buffer, "join ");

	send(socketInput, buffer, (int)strlen(buffer), 0);

	while (canJoin == false) {
		memset(buffer, 0, MAX_MESSAGE_SIZE);//czyszczenie bufora
		recv(socketInput, buffer, MAX_MESSAGE_SIZE, 0);
		printf("client: server respone: %s\n", buffer);
		if (strcmp(buffer, "OK JOIN") == 0) {
			memset(buffer, 0, MAX_MESSAGE_SIZE);//czyszczenie bufora
			strcat(buffer, "OK JOINING");
			send(socketInput, buffer, (int)strlen(buffer), 0);//wysylanie informacji, by serwer wiedzia?, ?e klient ca?y jest pod??czony
			printf("client: joined: \n");
			return 0;
		}
		else if (strncmp(buffer, "NO FREE TABLE", strlen("NO FREE TABLE")) == 0) {
				printf("client: no free table\n");
				return FREE_TABLE_NOT_FOUND;
		}
		if (strncmp(buffer, "WAIT_FOR_FREE_SLOT", 18) == 0) {
			//wydrukowa? info,
			//?e trzeba chwil? poczeka?
			printf("Need to wait\n");
		}
		
	}

	return 0;

}

/// <summary>
/// wysy?a wci?niey klawisz do serwera
/// </summary>
/// <param name="socketInput">socket po??czeniowy</param>
/// <param name="key">char wci?ni?tego znaku</param>
void sendInput(SOCKET socketInput, char key) {
	char buffer[MAX_MESSAGE_SIZE];
	strcpy(buffer, "INPUT ");

	strncat(buffer, &key, 1);

	send(socketInput, buffer, (int)strlen(buffer), 0);
}


/// <summary>
/// zczytuje r??nice planszy z wiadomosci i wysy?a do gry
/// </summary>
/// <param name="gamePtr"> wska?nik na gr?</param>
void getDiff(char buffer[MAX_MESSAGE_SIZE], Game* gamePtr) {
	int N = buffer[5] - '0';//zalozenie ze liczba graczy jest jednocyfrowa
	int bufIndex = 6;

	for (int i = 0; i < N; i++) {
		if (buffer[i] != ' ') {
			int x, y, val;

#ifdef TEST_OUTPUT
			printf("buf=%d\n", bufIndex);
#endif // TEST_OUTPUT

			x = atoi(&buffer[bufIndex]);
			bufIndex = getNextSpaceBar(buffer, bufIndex);
			y = atoi(&buffer[bufIndex]);
			bufIndex = getNextSpaceBar(buffer, bufIndex);
			val = atoi(&buffer[bufIndex]);
			bufIndex = getNextSpaceBar(buffer, bufIndex);
#ifdef TEST_OUTPUT
			printf("x=%d,y=%d,val=%d\n", x, y, val);
#endif // TEST_OUTPUT

			//board->tile[y][x] = val;
			gamePtr->AddToRedraw(Point(x, y), val);

		}
	}
}


void initBoard(char buffer[MAX_MESSAGE_SIZE], Game* gamePtr) {
	int tableSize;
	int bufIndex = 6;
	tableSize = atoi(&buffer[bufIndex]);				//pobranie Mapsize
	bufIndex = getNextSpaceBar(buffer, bufIndex);


	gamePtr->SetMapSize(tableSize);

	//odczytanie liczby graczy
	int maxPlayer;
	maxPlayer = atoi(&buffer[bufIndex]);				//pobranie maxPlayer
	bufIndex = getNextSpaceBar(buffer, bufIndex);

	//odczytanie koloru gracza   playerIndex = kolor gracza-1
	int playerColor;
	playerColor = atoi(&buffer[bufIndex]);				//pobranie playerIndex
	playerColor++;
	

	//TODO 
	//WYRYSOWANIE KOLORU GDZIE? NA EKRANIE
	//printf("kolor = %d", playerColor);
	//

	bufIndex = getNextSpaceBar(buffer, bufIndex);

	for (int i = 0; i < maxPlayer; i++) {
		Point pos;
		pos.x = atoi(&buffer[bufIndex]);				//pobranie pos
		bufIndex = getNextSpaceBar(buffer, bufIndex);
		pos.y = atoi(&buffer[bufIndex]);				//pobranie pos
		bufIndex = getNextSpaceBar(buffer, bufIndex);		
		gamePtr->AddToRedraw(Point(pos.x, pos.y), i + 1);
	}

	gamePtr->DrawMap();
	gamePtr->PrintPlayer(playerColor);
}


void startGame(bool*  gameStarted) {

	//je?li dasz t? zmienn? do klasy gra, to usu? w 
	//funkcji turnOnReceiveSocket() zmienn? gameStarted i w structs.h ze struktury Argument
	*gameStarted = 1;
}
void endGame(char buffer[MAX_MESSAGE_SIZE]) {
	bool won = buffer[9] - '0'; //zrzutowanie 1 na true, i 0 na false
	if (won) {
		//wygrana
		printf("YOU WON");
	}
	else {
		//przegrana
		printf("YOU LOST");
	}
}

/// <summary>
/// w?tek nas?uchuj?cy, modyfikuj?cy dane w tle
/// </summary>
/// <param name="arg"></param>
/// <returns></returns>
DWORD WINAPI turnOnReceiveSocket(void* arg) {
	auto socketConnected = true; //stan polaczenia
	//wczytanie argument?w
	auto socketOutput = ((Argument*)arg)->socketOutput;
	auto gameStarted = ((Argument*)arg)->gameStarted;
	auto gamePtr = ((Argument*)arg)->game;
	//auto gamePtr = *game

	delete arg;

	while (socketConnected) {//jest utrzymane po??czenie
		char buffer[MAX_MESSAGE_SIZE] = {};
		if (recv(socketOutput, buffer, MAX_MESSAGE_SIZE, 0) == 0) { //jesli nic nie odebrano(czyli blad)
			//to znaczy ze po??czenie zosta?o zerwane
			socketConnected = false;
		}

#ifdef TEST_OUTPUT
		printf("cl_rec_sock: %s\n", buffer); //kontrolny printf
#endif // TEST_OUTPUT

		//sygnal zmiany planszy		
		if (strncmp(buffer, "DIFF", 4) == 0) {
			getDiff(buffer, gamePtr);
		}
		//INIT BOARD
		else if (strncmp(buffer, "BOARD", 5) == 0) {
			initBoard(buffer, gamePtr);
		}
		else if (strncmp(buffer, "START_GAME", 10) == 0) {
			startGame(gameStarted);			
		}
		else if (strncmp(buffer, "END_GAME", 8) == 0) {
			endGame(buffer);
			closesocket(socketOutput); //zamkni?cie socketu
		}
	}
	return NULL;
}


/// <summary>
/// funkcja tworz?ca nowy w?tek, bo przesy?anie wielu argument?w to wrz?d na dupie
/// </summary>
/// <param name="socket">socket komunikacyjny</param>
/// <param name="board">adres sto?u</param>
/// <param name="player">adres gracza(</param>
/// <param name="gameStarted">adres bola czy gra sie ma juz zaczac</param>
/// <param name="sem">adres semafora</param>
void CreateReceiveSocket(SOCKET socket, bool* gameStarted, mutex* sem, Game* game) {
	DWORD thread_id;

	Argument* arg = (Argument*)malloc(sizeof(Argument));

	if (arg == NULL) {
		return;
	}
	arg->socketOutput = socket;
	arg->gameStarted = gameStarted;

	arg->game = game;

	CreateThread(0, NULL, turnOnReceiveSocket, (void*)arg ,0, &thread_id);
}


