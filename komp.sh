

cd Server

gcc -Wall -c server.c -o server.o -lm
gcc -Wall -c serverReceive.c -o serverReceive.o
gcc server.o serverReceive.o -o server -lm

cd ../


cd Client
gcc -Wall -c connect.c -o connect.o -lm
gcc -Wall -c client.c -o client.o -lm
gcc client.o connect.o -o client -lm





