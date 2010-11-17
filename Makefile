buffer.o: buffer.c buffer.h
	gcc -Wall -g -c buffer.c -o buffer.o
server.o: server.c server.h
	gcc -Wall -g -c server.c -o server.o
client.o: client.c client.h
	gcc -Wall -g -c client.c -o client.o
parse.o: parse.c parse.h
	gcc -Wall -g -c parse.c -o parse.o
main: main.c buffer.o client.o server.o parse.o
	gcc -Wall -g main.c buffer.o parse.o client.o server.o -o main
