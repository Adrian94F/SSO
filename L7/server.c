// Proces odbierajacy komunikaty - wysyla udp_cli
// Wspolpracuje z udp_cli
// Kompilacja gcc udp_serw.c -o udp_serw -lrt
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h> 
#define KROKI 10
#define PORT 9999
#define SIZE 512

typedef enum { false, true } bool;

bool enable = true;

typedef enum {
	OPENR,
	READ,
	SHUTDOWN,
	SAVE, 
	CLOSE
} type;

typedef struct {
	type typ;
	char buf[SIZE];
	int handle;
	int ile;
} msg_t;

void blad(char *s) {
	perror(s);
	exit(1);
}

int main(void) {
	struct sockaddr_in adr_moj, adr_cli;
	int s, i, slen=sizeof(adr_cli),snd, ile, blen=sizeof(msg_t);
	char buf[SIZE];
	msg_t msg;

	gethostname(buf,sizeof(buf));
	printf("Host: %s\n",buf);

	s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(s < 0) blad("socket");
	printf("Gniazdko %d utworzone\n",s);
	// Ustalenie adresu IP nadawcy
	memset((char *) &adr_moj, 0, sizeof(adr_moj));
	adr_moj.sin_family = AF_INET;
	adr_moj.sin_port = htons(PORT);
	adr_moj.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(s,(struct sockaddr *) &adr_moj, sizeof(adr_moj))==-1)
		blad("bind");

	bool save = false;
	int handle;

	do {
		//odczyt zapytania
		ile = recvfrom(s, &msg, blen, 0,(struct sockaddr *) &adr_cli, &slen);
		if(ile < 0) blad("ilevfrom()");
		printf(
			"Odebrano komunikat z %s:%d o rozmiarze %d\n",
			inet_ntoa(adr_cli.sin_addr), 
			ntohs(adr_cli.sin_port), 
			ile);

		//wykonanie
		switch (msg.typ) {
			case OPENR:
				msg.handle = open(msg.buf, O_RDWR);
				//odpowiedź
				sprintf(msg.buf, "%d", msg.handle);
				break;
			case READ:
				msg.ile = read(msg.handle, msg.buf, SIZE);
				break;
			case CLOSE:
				close(msg.handle);
				break;
			case SAVE:
				if (save) {

				} else {
					handle = open(msg.buf, O_RDWR | O_CREAT);
				}
				break;
			case SHUTDOWN:
				enable = false;
				//odpowiedź
				sprintf(msg.buf, "OK! Shutting down.");
				break;
		}
		snd = sendto(s, &msg, blen, 0,(struct sockaddr *) &adr_cli, slen);
		if(snd < 0) 
			blad("sendto()");
		printf("wyslano odpowiedz o rozmiarze %d\n",snd);

	}
	while (enable);

	close(s);
	return 0;
}

