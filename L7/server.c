#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/time.h>
#include <errno.h>

#define BUFLEN 80
#define KROKI 10
#define PORT 9865
#define LKLIENTOW 10

typedef struct 
{
	char buf[BUFLEN];
} msgt;

typedef struct
{
	int sock;
	int threadnum;
} arg;

void* client(void* _thread_args)
{
	printf("+++ Robię nowy wątek...\n");
	fflush(0);

	arg thread_args;
	memcpy(&thread_args, _thread_args, sizeof(arg));
	int s = thread_args.sock;
	int threadnum = thread_args.threadnum;

	msgt msg;
	int rec;
	fflush(0);

	do
	{
		if((rec = recv(s, &(msg.buf), sizeof(msg.buf), 0)) < 0)
		{
			perror(" nie odczytałem z gniazdka");
			close(s);
			exit(EXIT_FAILURE);
		}
		else if(rec == 0)
		{
			printf("+++ Klient %d rozłączył się\n", threadnum);
			shutdown(s, SHUT_RDWR);
			return 0;
		}
		else
		{
			printf("    K%d: \"%s\"\n", threadnum, msg.buf);
			fflush(0);
			sprintf(msg.buf ,"Odpowiedź od serwera");
			if((rec = send(s, &(msg.buf), sizeof(msg.buf), 0)) < 0)
			{
				perror(" nie wysłałem odpowiedzi");
				close(s);
				exit(EXIT_FAILURE);
			}
		}
	} while(1);
	printf("Koniec.\n");
	fflush(0);  
}

int main(int argc,char ** argv) 
{
	pthread_t clients[LKLIENTOW];
	unsigned accepted;
	socklen_t clen;
	struct sockaddr_in serv_addr;
	int sock, port;
	arg thread_args;

	port = argc > 1 ? atoi(argv[1]) : PORT;											// port z argumentu

	if ((sock=socket(AF_INET, SOCK_STREAM, 0)) < 0)									// stwórz gniazdko
	{
		perror(" nie stworzyłem gniazdka");
		exit(EXIT_FAILURE);
	}

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(port);

	if (bind(sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0)
	{
		perror(" nie nadałem gniazdku adresu");
		exit(EXIT_FAILURE);
	}
	listen(sock, 123);
	/*	
		od tej chwili wszystkie próby połączenia od klientów nie będą w programach
		do czasu, aż serwer zaakceptuje poszczególne połączenia funkcją accept()
		Dr inż. Tomasz Surmacz 
	*/

	printf("+++ Nasłuchuję...\n");

	while(1)
	{
		if((accepted = accept(sock,(struct sockaddr *)&serv_addr,&clen)) < 0)		// czekaj na klienta
		{
			perror(" nie zaakceptowano");
			exit(EXIT_FAILURE);
		}

		thread_args.sock = accepted;
		thread_args.threadnum = i;

		if( pthread_create(&clients[i], NULL, client, (void *)(&thread_args)) != 0)	// stwórz wątek dla klienta
		{  
			perror(" nie stworzyłem wątku");
			exit(EXIT_FAILURE);
		}
		i++;
	}
	return 0;
}