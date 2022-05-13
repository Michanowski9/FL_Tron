#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>


void startListening(int listenSocket){

	char buffer[80];
	char key;
	for(int i=0;i<3;i++){ //socket otwarty
		memset(buffer,0,80);
		recv(listenSocket,buffer,80,0);		
		printf("socket received: %s\n",buffer);	
		
		if(strncmp(buffer,"INPUT",5) == 0){
			key=buffer[6];	
			switch(key){
		       		case('w'): printf("hit w!\n");break;
		       		case('s'): printf("hit s!\n");break;
		       		case('a'): printf("hit a!\n");break;
		       		case('d'): printf("hit d!\n");break;				       		case(' '): printf("hit  !\n");break;
			}
		}				
	}
	printf("socket close\n");
	close(listenSocket);

}


//wysyla aktualny stan lobby
void updateWaitingRoom(int sockets[], void* players){ //player nazwa,indeks w grze


}

//wysyla poczatkowy stan planszy
void sendInitialBoard(int sockets[], void* board){
}

//wysyla sygnal do rozpoczecia odliczenia
void startReadyCounter(int sockets[]){
}

//sygnal ze gra sie rozpoczela
void startGame(int sockets[]){
}

//wysylka zmian na planszy
void sendDifference(int sockets[], void* difference, int n){
}


