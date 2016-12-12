// Proces wysyla a potem odbiera komunikaty udp
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#define BUFLEN 80
#define LWIADOMOSCI 10
#define PORT 9865
#define SRV_IP "127.0.0.1"

typedef struct 
{
	char buf[BUFLEN];
} msgt;

int sock;

void intHandler(int signum)
{
	msgt msg;
	if(shutdown(sock, SHUT_WR) < 0)											// zamknij gniazdka do pisania
	{
		perror(" bład podczas zamykania gniazdka do pisania");
		exit(EXIT_FAILURE);
	}
	recv(sock, &(msg.buf), sizeof(msg.buf), 0);
	close(sock);
	printf("\n+++ Gniazdko do pisania zamknięte\n+++ Umieram...\n");
	exit(0);
}

int main(int argc, char * argv[]) 
{
	signal(SIGINT, intHandler);												// sygnał SIGINT

	struct sockaddr_in adr_serw;
	int i, snd, rec, port;
	msgt msg;

	port = argc > 1 ? atoi(argv[1]) : PORT;									// port z argumentu
		

	memset(&adr_serw, 0, sizeof(adr_serw));									// adres serwera
	adr_serw.sin_family = AF_INET;
	adr_serw.sin_addr.s_addr = inet_addr(SRV_IP);
	adr_serw.sin_port = htons(port);

	if ((sock=socket(AF_INET, SOCK_STREAM, 0)) < 0)							// stwórz gniazdko
	{
		perror(" nie można utworzyć gniazdka");
		exit(EXIT_FAILURE);
	}
	printf("+++ Utworzyłem gniazdko %d\n",sock);

	if (connect(sock, (struct sockaddr*) &adr_serw, sizeof(adr_serw)) < 0)	// połącz z serwerem
		perror(" nie można połączyć się z serwerem");

	for (i = 0; i < LWIADOMOSCI; i++) 										// ślij wiadomości co sekundę
	{
		sprintf(msg.buf, "Wiadomość nr %d, od klienta", i);
		if((snd = send(sock, &msg.buf, sizeof(msg.buf), 0)) < 0)
		{
			perror(" nie można wysłać wiadomości");
			exit(EXIT_FAILURE);
		}
		printf("+++ Wysłałem komunikat nr %d\n+++ Czekam na odpowiedź\n", i);
		
		if((rec = recv(sock, &msg.buf, sizeof(msg.buf), 0)) < 0)
		{
			perror(" nie można odebrać wiadomości");
			exit(EXIT_FAILURE);
		}
		printf("    \"%s\"\n", msg.buf);
		sleep(1);
	}
	intHandler(0);															// zamknij gniazdka
	return 0;
}