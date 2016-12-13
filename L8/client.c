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

int sock, sock2;
char hello[] = "Dzień dobry! Witam! Szczęść Boże!";
int i, snd, rec, port, port2;
msgt msg;
struct sockaddr_in adr_serw, serv_addr, cl_addr;
struct hostent *hname;

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
	close(sock2);
	printf("\n+++ Gniazdko do pisania zamknięte\n+++ Umieram...\n");
	exit(0);
}

int main(int argc, char * argv[]) 
{
	
	signal(SIGINT, intHandler);												// sygnał SIGINT

	port = argc > 1 ? atoi(argv[1]) : PORT;									// port z argumentu
	port2 = port + 1;

	memset(&adr_serw, 0, sizeof(adr_serw));									// adres serwera TCP
	adr_serw.sin_family = AF_INET;
	adr_serw.sin_addr.s_addr = inet_addr(SRV_IP);
	adr_serw.sin_port = htons(port);

	memset(&serv_addr, 0, sizeof(serv_addr));								// adres serwera UDP
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(SRV_IP);
	serv_addr.sin_port = htons(port2);

	if (argc>1) 
	{
		hname=(gethostbyname("localhost"));
		if (hname != NULL) 
		{
			memcpy(&serv_addr.sin_addr, hname->h_addr_list[0], hname->h_length);
		}
	}

	memset(&cl_addr, 0, sizeof(cl_addr));									// adres klienta
	cl_addr.sin_family = AF_INET;
	cl_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	cl_addr.sin_port = htons(0);

	if (bind(sock2, (struct sockaddr*) &cl_addr, sizeof(cl_addr)) < 0)		// gniazdko UDP
	{
		perror(" nie nadałem gniazdku UDP adresu");
		exit(EXIT_FAILURE);
	}

	if (sendto(sock2, hello, strlen(hello), &serv_addr, sizeof(serv_addr), 0) != strlen(hello))
	{
		perror(" nie można wysłać wiadomości");
		exit(EXIT_FAILURE);
	}

	if ((sock=socket(AF_INET, SOCK_STREAM, 0)) < 0)							// stwórz gniazdko TCP
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