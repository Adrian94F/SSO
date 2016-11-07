#include "header.h"

int getch()
{
	int key;
	system("stty raw -echo");
	do
	{
		key = getchar();
	}
	while(key == '\n');
	system ("stty cooked");
	return key;
}

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
	int msgqid, rc;

	// Utworzenie kolejki
	msgqid = msgget(IPC_PRIVATE, MSGPERM|IPC_CREAT|IPC_EXCL);
	if (msgqid < 0)
	{
		perror(strerror(errno));
		printf("Błąd tworzenia kolejki o msgqid = %d\n", msgqid);
		return 1;
	}
	printf("Kolejka %d utworzona\n",msgqid);

	getch();

	for (int i = 0; i < n; i++)
	{
		// Wiadomość do wysłania
		msg.mtype = type > 0 ? type : (rand() % 10);
		sprintf (msg.mtext, "%s %d", "wiadomość", i);

		// Wysłanie komunikatu do kolejki
		rc = msgsnd(msgqid, &msg, sizeof(msg.mtext), 0);
		if (rc < 0) 
		{
			perror(strerror(errno));
			printf("Błąd wysyłania %d\n [%ld] \"%s\"", rc, msg.mtype, msg.mtext);
			return 1;
		}
		else
			printf("Wysłano [%ld] \"%s\"\n", msg.mtype, msg.mtext);

	}
	return 0;
}