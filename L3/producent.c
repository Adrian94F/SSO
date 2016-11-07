#include "header.h"

int msgqid, rc;
int done;

/*
	ARGV:	1: typ komunikatu (0: random, >0 konkretny typ)
			2: liczba komunikatów
*/
int main(int argc,char **argv)
{
	if (argc < 3)
	{
		printf("Błąd!\n producent [typ_komunikatu] [liczba komunikatów]\n");
		return 1;
	}

	srand(time(NULL));
	int type = atoi(argv[1]);
	int n = atoi(argv[2]);

	// Utworzenie kolejki
	msgqid = msgget(IPC_PRIVATE, MSGPERM|IPC_CREAT|IPC_EXCL);
	if (msgqid < 0) {
		perror(strerror(errno));
		printf("Błąd tworzenia kolejki o msgqid = %d\n", msgqid);
		return 1;
	}
	printf("Kolejka %d utworzona\n",msgqid);
	
	for (int i = 0; i < n; i++)
	{
		// Wiadomość do wysłania
		msg.mtype = type > 0 ? type : rand() % 10; 			// Ustawienie typu komunikatu
		sprintf (msg.mtext, "%s\n", "wiadomość do wysłania");

		// Wysłanie komunikatu do kolejki
		rc = msgsnd(msgqid, &msg, sizeof(msg.mtext), 0);	// Ostatni parametr: 0, IPC_NOWAIT, MSG_NOERROR lub IPC_NOWAIT|MSG_NOERROR
		if (rc < 0) {
			perror( strerror(errno) );
			printf("Błąd wysyłania %d\n", rc);
			return 1;
		}

	}
	

	// Odczytanie komunikatu z kolejki
	rc = msgrcv(msgqid, &msg, sizeof(msg.mtext), 0, 0); 
	if (rc < 0) {
		perror( strerror(errno) );
		printf("Błąd odczytu %d\n", rc);
		return 1;
	} 
	printf("Odebrano: %s\n", msg.mtext);

	// Usunięcie kolejki
	rc=msgctl(msgqid,IPC_RMID,NULL);
	if (rc < 0) {
		perror( strerror(errno) );
		printf("Błąd usuwania kolejki%d\n", rc);
		return 1;
	}
	printf("Kolejka %d usunięta\n",msgqid);

	return 0;
}