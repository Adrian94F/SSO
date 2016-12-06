#include "header.h"
main (int argc,char ** argv)
{
	int sock, n, nw;
	struct sockaddr_in serv_addr;
	char *serv_ip;
	int serv_port;
	char buf[1024];
	if (argc>=3) 
	{
		serv_ip = argv[1]; 
		serv_port = atoi(argv[2]);
	} 
	else 
	{
		serv_ip = SERVADDR; serv_port = SERVPORT;
	}
	/* ustalenie adresu serwera */
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(serv_ip);
	serv_addr.sin_port = htons(serv_port);
	if ((sock=socket(AF_INET, SOCK_STREAM, 0)) < 0)
		perror("klient: nie można utworzyć gniazdka");
	if (connect(sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0)
		perror("klient: nie można połączyć się z serwerem");
	/* od tego momentu gniazdko sock jest połączone i możemy je traktować
	   tak, jak każdy inny deskryptor pliku czy strumienia pipe/fifo
	*/
	/* czytanie/pisanie danych do serwera */
	while ((n=read(sock, buf, sizeof(buf))) >0) 
	{
		sprintf(&buf, "coś bardzo waznego");
		nw = write(sock, buf, sizeof(buf));
		if (0 /* koniec połączenia inicjowany przez klienta: */) 
		{
			write(sock, "koniec\n", 8);
			shutdown(sock, 1); /* nie będziemy już pisać, oczekujemy nadal na EOF */
		}
	}
	if (n==0) 
	{ /* EOF */
		shutdown(sock, 2);
		close(sock);
	} 
	else 
	{
		perror("błąd funkcji read()");
	}
}