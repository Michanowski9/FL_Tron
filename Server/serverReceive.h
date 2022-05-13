

void startListening(int listenSocket);

void updateWaitingRoom(int sockets[], void* players); //player nazwa,indeks w grze

void sendInitialBoard(int sockets[], void* board);

void startReadyCounter(int sockets[]);

void startGame(int sockets[]);

void sendDifference(int sockets[], void* difference, int n);

//differnce to cos takiego: [(x1,y1,wartosc_pola),(x2,y2,wartosc_pola),...]
//n-jak duzo roznic (bo zalezy od liczby zyjacych graczy
