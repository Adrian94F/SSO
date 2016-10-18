CC = gcc
CFLAGS = -Wall -pedantic

all: pipew piper muzikplejer

pipew: pipew.c
	$(CC) -o $@ $< $(CFLAGS)
piper: piper.c
	$(CC) -o $@ $< $(CFLAGS)
muzikplejer: muzikplejer.c
	$(CC) -o $@ $< $(CFLAGS)

clean:
	rm pipew piper muzikplejer

