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


pthread_t clients[LKLIENTOW];
pthread_t servUDP;
unsigned accepted;
socklen_t clen, clen2;
struct sockaddr_in serv_addr, serv_addr_2, cl_addr, cl_addr_2;
int sock, port, port2, i = 0, sock2, cl_len_2;
arg thread_args;
char * buffer[BUFLEN];
int c = 0;


void * client(void * _thread_args)
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

	while(i==0);
	//i--;
		
	if((rec = sendto(s, &(msg.buf), sizeof(msg.buf), &cl_addr_2, cl_len_2, 0)) < 0)
	{
		perror(" nie wysłałem odpowiedzi");
		close(s);
		exit(EXIT_FAILURE);
	}

	do
	{
		
		if((rec = recv(s, &(msg.buf), sizeof(msg.buf), 0)) < 0)
		{
			perror(" nie odczytałem z gniazdka TCP");
			close(s);
			exit(EXIT_FAILURE);
		}
		if(rec == 0)
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

void * server(void * _thread_args)
{
	printf("+++ Rozczłonkowałem się, server UDP działa...\n");
	while(1)
	{
		if ((recvfrom(sock2, buffer, sizeof(buffer), 0, &cl_addr_2, &cl_len_2)) < 0)
		{
			perror(" nie odczytałem z gniazdka UDP");
			exit(EXIT_FAILURE);
		}
		printf("+++ Obebrałem wiadomość \"%s\", wysyłam ją do wszystkich\n", buffer);
		c = i;
	}

}

int main(int argc,char ** argv) 
{

	port = argc > 1 ? atoi(argv[1]) : PORT;											// port z argumentu
	port2 = port + 1;

	if ((sock=socket(AF_INET, SOCK_STREAM, 0)) < 0)									// stwórz gniazdko
	{
		perror(" nie stworzyłem gniazdka TCP");
		exit(EXIT_FAILURE);
	}
	if ((sock2=socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror(" nie stworzyłem gniazdka UDP");
		exit(EXIT_FAILURE);
	}

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(port);

	memset(&serv_addr_2, 0, sizeof(serv_addr_2));
	serv_addr_2.sin_family = AF_INET;
	serv_addr_2.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr_2.sin_port = htons(port2);

	if (bind(sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0)
	{
		perror(" nie nadałem gniazdku TCP adresu");
		exit(EXIT_FAILURE);
	}
	if (bind(sock2, (struct sockaddr*) &serv_addr_2, sizeof(serv_addr_2)) < 0)
	{
		perror(" nie nadałem gniazdku UDP adresu");
		exit(EXIT_FAILURE);
	}
	cl_len_2=sizeof(cl_addr);

	thread_args.sock = sock2;
	thread_args.threadnum = 0;

	if( pthread_create(&servUDP, NULL, server, (void *)(&thread_args)) != 0)	// stwórz wątek dla klienta
	{  
		perror(" nie stworzyłem wątku");
		exit(EXIT_FAILURE);
	}

	listen(sock, 5);
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