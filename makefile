CC1=gcc

CFLAGS = -g -c -Wall
#CFLAGS = -ansi -c -Wall -pedantic
all: client server client2

A = client.o
B = server.o
C = client2.o

client2.o: client2.c
	 $(CC1) -c client2.c

client2: $C
	 $(CC1) -pthread -o client2 $C

client.o: client.c
	$(CC1) -c client.c

client: $A
	$(CC1) -pthread -o client $A

server.o: server.c
	$(CC1) -c server.c

server: $B
	$(CC1) -pthread -o server $B

clean:
	rm *.o
	rm server
	rm client
	rm client2
