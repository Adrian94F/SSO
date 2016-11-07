CC = gcc
CFLAGS = -Wall -pedantic

all: producent konsument

producent: producent.c
	$(CC) -o $@ $< $(CFLAGS)
konsument: konsument.c
	$(CC) -o $@ $< $(CFLAGS)

clean:
	rm producent konsument
