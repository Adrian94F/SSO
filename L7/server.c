#include "header.h"
main (int argc,char ** argv) 
{
	int sock, newsock, pid, clen, port;
	char buf[1024];
	struct sockaddr_in cl_addr, serv_addr;
	/* odczytanie numeru portu z linii komend */
	if (argc>1)
		port = atoi(argv[1]);
	else
		port = SERVPORT;
	/* nadanie adresu gniazdku */
	if ((sock=socket(AF_INET, SOCK_STREAM, 0)) < 0)
		perror("serwer: nie można utworzyć gniazdka");
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(port);
	if (bind(sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0)
		perror("serwer: nie można nadać gniazdku adresu");
	listen(sock, 5);
	/* od tej chwili wszystkie próby połączenia od klientów nie będą w programach
	* klientów zwracały błędu "Connection refused", lecz * programy te będą blokowane
	* do czasu, aż serwer zaakceptuje poszczególne * połączenia funkcją accept()
	*/
	while(1) 
	{ /* oczekiwanie na połączenia z klientami */
		clen = sizeof(cl_addr);
		newsock = accept(sock, (struct sockaddr*) &cl_addr, &clen);
		if (newsock < 0)
			perror("serwer: błąd w funkcji accept()");
		else 
		{
			switch (pid=fork()) 
			{
				case 0: /* dziecko -- obsługuje dopiero co nawiązane połączenie */
					close (sock);
					read(newsock, buf, sizeof(buf)); 
					write(newsock, "received: %s", buf);
					shutdown(newsock, 2);
					close(newsock);
					exit(0);
				case (-1):
					perror("błąd podczas fork()"); 
					break;
			} /* switch */
		} /* else -- rodzic */
		close (newsock); /* rodzic wraca do oczekiwania na nowe połączenia */
	} /* while */
} /* main */