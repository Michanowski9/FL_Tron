funkcje które będą wykorzystywane przez którąś ze stron


Klient:
 -connectToServer()
 -join()
 -createRoom()
 -turnOnReceiveSocket();
 -sendInput()

Serwer(a wlaściwie stół):
 -updateWaitingRoom()
 -sendInitialBoard()
 -startReadyCounter()
 -startGame()
 -sendDifference()




Wiadomości:



DEATHS N 0 2 3  //N-ile, a potem ktory gracz

DIFF N 0 1 2 2 3 5 // N-ile krotek, a potem krotki trzech wartosci (x,y,val)

PLAYERS N nick1 nick2 ... nickN  //update lobby


START_COUNTER

START_GAME
