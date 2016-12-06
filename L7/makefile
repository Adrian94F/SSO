CC = gcc
CFLAGS = -Wall -pedantic -g -lm -lrt -lpthread

all: server client serverJ clientJ

server: server.c
	$(CC) -o $@ $< $(CFLAGS)

clean:
	rm server client serverJ clientJ
