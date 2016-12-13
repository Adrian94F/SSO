CC = gcc
CFLAGS = -Wall -pedantic -g -lm -lrt -lpthread

all: server client 

server: server.c
	$(CC) -o $@ $< $(CFLAGS)
client: client.c
	$(CC) -o $@ $< $(CFLAGS)

clean:
	rm server client 
