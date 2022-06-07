#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "connect.h"

#define MAX_MESSAGE_SIZE 80
#define HTONS 3001
//#define INET_ADDR "127.0.1.1"
#define INET_ADDR "192.168.0.87"
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

	return socketInput;

}


//sygnal wysylany do serwera, ze gracz chce
//dolaczyc do dowolnego stolu
//zwraca 0 gdy sie udalo
//w przeciwnym razie FREE_TABLE_NOT_FOUND
int join(int socketInput) {
	char buffer[MAX_MESSAGE_SIZE];
	strcpy(buffer, "join ");

	send(socketInput, buffer, strlen(buffer), 0);

	memset(buffer, 0, MAX_MESSAGE_SIZE);//czyszczenie bufora
	recv(socketInput, buffer, MAX_MESSAGE_SIZE, 0);
	printf("client: server respone: %s\n", buffer);
	if (strcmp(buffer, "OK JOIN") == 0) {
		printf("client: joined: \n");
	}
	else {
		if (strncmp(buffer, "NO FREE TABLE", strlen("NO FREE TABLE")) == 0) {
			printf("client: no free table\n");
			return FREE_TABLE_NOT_FOUND;
		}
	}


	return 0;

}

/// <summary>
/// wysy³a wciœniey klawisz do serwera
/// </summary>
/// <param name="socketInput">socket po³¹czeniowy</param>
/// <param name="key">char wciœniêtego znaku</param>
void sendInput(SOCKET socketInput, char key) {
	char buffer[MAX_MESSAGE_SIZE];
	strcpy(buffer, "INPUT ");

	strncat(buffer, &key, 1);

	send(socketInput, buffer, strlen(buffer), 0);
}


/// <summary>
/// w¹tek nas³uchuj¹cy, modyfikuj¹cy dane w tle
/// </summary>
/// <param name="arg"></param>
/// <returns></returns>
DWORD WINAPI turnOnReceiveSocket(void* arg) {
	bool socketConnected = true; //stan polaczenia
	//wczytanie argumentów
	Board* board = ((Argument*)arg)->board;
	int socketOutput = ((Argument*)arg)->socketOutput;
	Player* player = ((Argument*)arg)->player;
	mutex* sem = ((Argument*)arg)->sem;
	bool* gameStarted;
	gameStarted = ((Argument*)arg)->gameStarted;

	free((Argument*)arg); //zwolnienie pamiêci 

	char buffer[MAX_MESSAGE_SIZE];
	while (socketConnected) {//jest utrzymane po³¹czenie
		memset(buffer, 0, MAX_MESSAGE_SIZE);//czyszczenie bufora
		if (recv(socketOutput, buffer, MAX_MESSAGE_SIZE, 0) == 0) { //jesli nic nie odebrano(czyli blad)
			//to znaczy ze po³¹czenie zosta³o zerwane
			socketConnected = false;
		}
		printf("cl_rec_sock: %s\n", buffer); //kontrolny printf

		//sygnal zmiany planszy		
		if (strncmp(buffer, "DIFF", 4) == 0) {
			int N = buffer[5] - '0';//zalozenie ze liczba graczy jest jednocyfrowa
			int bufIndex = 6;
			sem->lock();//LOCK
			for (int i = 0; i < N; i++) {
				if (buffer[i] != ' ') {
					int x, y, val;
					x = atoi(&buffer[bufIndex]);

					printf("buf=%d\n", bufIndex);

					bufIndex = getNextSpaceBar(buffer, bufIndex);
					y = atoi(&buffer[bufIndex]);

					bufIndex = getNextSpaceBar(buffer, bufIndex);
					val = atoi(&buffer[bufIndex]);


					printf("x=%d,y=%d,val=%d\n", x, y, val);
					board->tile[y][x] = val;
				}
				bufIndex++;
			}
			sem->unlock();//UNLOCK						

		}
		//INIT BOARD
		else if (strncmp(buffer, "BOARD", 5) == 0) {
			int tableSize = TABLE_SIZE;
			//rezerwacja miejsca na stol
			board->tile = (int**)malloc(sizeof(int*) * tableSize);
			for (int i = 0; i < tableSize; i++) {
				board->tile[i] = (int*)malloc(sizeof(int) * tableSize);
				for (int j = 0; j < tableSize; j++) {
					board->tile[i][j] = 0;
				}
			}
			board->size = tableSize;


			//ustawienie graczy na poczatku
			Position pos;
			int bufIndex = 6;			
			board->players = (Player**)malloc(sizeof(Player*)*MAX_PLAYERS);//rezerwacja miejsca na wskazniki na graczy
			for (int i = 0; i < MAX_PLAYERS; i++) {				
				board->players[i] = new Player();//rezerw. miejsca na gracza
				pos.x = atoi(&buffer[bufIndex]);				//pobranie pos
				bufIndex = getNextSpaceBar(buffer, bufIndex); 
				pos.y = atoi(&buffer[bufIndex]);				//pobranie pos
				board->players[i]->pos = pos;   //przypisanie pos

				board->tile[pos.y][pos.x] = i+1; //aktualizacja mapy
			}

		}
		else if (strncmp(buffer, "START_GAME", 10) == 0) {
			sem->lock();//LOCK
			*gameStarted = true;
			sem->unlock();//UNLOCK
		}
	}
	return NULL;
}

/// <summary>
/// funkcja tworz¹ca nowy w¹tek, bo przesy³anie wielu argumentów to wrzód na dupie
/// </summary>
/// <param name="socket">socket komunikacyjny</param>
/// <param name="board">adres sto³u</param>
/// <param name="player">adres gracza(</param>
/// <param name="gameStarted">adres bola czy gra sie ma juz zaczac</param>
/// <param name="sem">adres semafora</param>
void CreateReceiveSocket(SOCKET socket, Board* board, Player* player, bool* gameStarted, mutex* sem) {
	DWORD thread_id;

	Argument* arg = (Argument*)malloc(sizeof(Argument));
	arg->board = board;
	arg->sem = sem;
	arg->socketOutput = socket;
	arg->player = player;
	arg->gameStarted = gameStarted;
	CreateThread(0, NULL, turnOnReceiveSocket, (void*)arg ,0, &thread_id);
}


