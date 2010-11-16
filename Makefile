buffer.o: buffer.c buffer.h
	gcc -Wall -g -c buffer.c -o buffer.o
serverinfo.o: serverinfo.c serverinfo.h
	gcc -Wall -g -c serverinfo.c -o serverinfo.o
client.o: client.c client.h
	gcc -Wall -g -c client.c -o client.o
main: main.c buffer.o client.o serverinfo.o
	gcc -Wall -g main.c buffer.o client.o serverinfo.o -o main
clean:
	rm -f *.o
