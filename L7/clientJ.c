#include "header.h"

void blad(char *s) 
{
	perror(s);
	exit(1);
}

struct sockaddr_in adr_moj, adr_serw, adr_x;
int s, i, slen, snd, blen,rec;
char buf[SIZE];
msg_t msg;

int main(int argc, char * argv[]) 
{
	if (argc > 2) //adres + typ operacji (+ ew. parametry)
	{
		slen=sizeof(adr_serw);
		blen=sizeof(msg_t);
	
		s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if(s < 0) 
			blad("socket");
		printf(" ### Gniazdko %d utworzone\n",s);
		memset((char *) &adr_serw, 0, sizeof(adr_serw));
		adr_serw.sin_family = AF_INET;
		adr_serw.sin_port = htons(PORT);
		if (inet_aton(argv[1], &adr_serw.sin_addr)==0) 
		{
			fprintf(stderr, "inet_aton() failed\n");
			_exit(1);
		}
	
		int handle;

		switch(argv[2][0]) 
		{
			case '1':												//otwórz plik
				if (argc > 3) 
				{
					//przekaż nazwę pliku
					msg.typ = OPENR;
					sprintf(msg.buf, "%s", argv[3]);		
					snd = sendto(s, &msg, blen, 0,(struct sockaddr *) &adr_serw,(socklen_t) slen);
					if(snd < 0) 
						blad("sendto()");
					printf(" ### Wysłano komunikat: %d\n", snd);

					//odbierz uchwyt
					printf(" ### Czekam na odpowiedź\n");
					rec = recvfrom(s, &msg, blen, 0,(struct sockaddr *) &adr_x,(socklen_t *) &slen);
					if(rec < 0) 
						blad("recvfrom()");
					printf(" ### Otrzymana odpowiedź %s\n",msg.buf);

					msg.typ = READ;
					do 
					{
						//wyślij uchwyt i polecenie odczytu
						snd = sendto(s, &msg, blen, 0,(struct sockaddr *) &adr_serw,(socklen_t) slen);
						if(snd < 0) 
							blad("sendto()");
						printf(" ### Wyslano komunikat: %d\n", snd);

						//odbierz dane
						printf(" ### Czekam na odpowiedz\n");
						rec = recvfrom(s, &msg, blen, 0,(struct sockaddr*) &adr_x,(socklen_t*) &slen);
						if(rec < 0) 
							blad("recvfrom()");
						printf(" ### Otrzymana odpowiedz\n%s\n",msg.buf);
					}
					while (msg.ile == SIZE);
					//wyślij uchwyt i polecenie zamknięcia pliku
					msg.typ = CLOSE;
					snd = sendto(s, &msg, blen, 0,(struct sockaddr *) &adr_serw,(socklen_t) slen);
					if(snd < 0) 
						blad("sendto()");
					printf(" ### Wyslano komunikat: %d\n", snd);

					//odbierz dane
					printf(" ### Czekam na odpowiedz\n");
					rec = recvfrom(s, &msg, blen, 0,(struct sockaddr*) &adr_x,(socklen_t*) &slen);
					if(rec < 0) 
						blad("recvfrom()");
					printf(" ### Otrzymana odpowiedz\n%s\n",msg.buf);

				}
				break;

			case '2':												//zapisz plik
				handle = open(argv[3], O_RDWR);
				sprintf(msg.buf, "%s", argv[3]);				//przekaż nazwę
				snd = sendto(s, &msg, blen, 0,(struct sockaddr *) &adr_serw,(socklen_t) slen);
				if(snd < 0) 
					blad("sendto()");
				printf(" ### Wyslano komunikat: %d\n", snd);
				do 
				{
					msg.ile = read(msg.handle, msg.buf, SIZE);	//odczyt pliku do bufora
					snd = sendto(s, &msg, blen, 0,(struct sockaddr *) &adr_serw,(socklen_t) slen);
					if(snd < 0) 
						blad("sendto()");
					printf(" ### Wyslano komunikat: %d\n", snd);
				}
				while (msg.ile == SIZE);
				break;

			case '3':												//wyłącz serwer
				//wyślij komunikat
				msg.typ = SHUTDOWN;
				sprintf(msg.buf, " ### Wysylam komunikat %d", i);
				snd = sendto(s, &msg, blen, 0,(struct sockaddr *) &adr_serw,(socklen_t) slen);
				if(snd < 0) 
					blad("sendto()");
				printf(" ### Wyslano komunikat: %d\n", snd);
				//czekaj na odpowiedź
				printf(" ### Czekam na odpowiedz\n");
				rec = recvfrom(s, &msg, blen, 0,(struct sockaddr *) &adr_x,(socklen_t *) &slen);
				if(rec < 0) 
					blad("recvfrom()");
				printf(" ### Otrzymana odpowiedz %s\n",msg.buf);
				break;
		}	
		close(s);
	}
	return 0;
}

