auction.o: auction.c auction.h
	gcc -Wall -g -c auction.c -o auction.o

buffer.o: buffer.c buffer.h
	gcc -Wall -g -c buffer.c -o buffer.o

commands.o: commands.c commands.h
	gcc -Wall -g -c commands.c -o commands.o

server.o: server.c server.h
	gcc -Wall -g -c server.c -o server.o

client.o: client.c client.h
	gcc -Wall -g -c client.c -o client.o

game.o: game.c game.h
	gcc -Wall -g -c game.c -o game.o

parse.o: parse.c parse.h
	gcc -Wall -g -c parse.c -o parse.o

print.o: print.c print.h
	gcc -Wall -g -c print.c -o print.o

main: main.c buffer.o client.o commands.o server.o parse.o print.o auction.o game.o 
	gcc -Wall -g main.c buffer.o parse.o client.o commands.o game.o server.o print.o auction.o -o main
	rm -f *.o

clean:
	rm -f *.o main
