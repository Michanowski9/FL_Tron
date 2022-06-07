
gcc -c stringFuncs.c


cd Server

gcc -Wall -c server.c -o server.o -lm
gcc -Wall -c serverReceive.c -o serverReceive.o
gcc -Wall -c table.c -o table.o
gcc server.o serverReceive.o ../stringFuncs.o table.o -o server -lm -lpthread



