all: server client
server: server.c
	gcc server.c -o server -g -lm -lrt -pthread
client: client.c
	gcc client.c -o client -g -lm -lrt -pthread
clean:
	rm init server client *.o