CC1=gcc

CFLAGS = -g -c -Wall
#CFLAGS = -ansi -c -Wall -pedantic
all: Client Server Client2

A = client.o
B = server.o
C = client2.o

client2: $C
	 $(CC1) -o client2 $C

client2.o: client2.c
	   $(CC1) -c client2.c

client: $A
	$(CC1) -o client $A

client.o: client.c
	$(CC1) -c client.c

server: $B
	$(CC1) -o server $B

server.o: server.c
	$(CC1) -c server.c

clean:
	rm *.o
	rm Server
	rm Client
	rm Client2
