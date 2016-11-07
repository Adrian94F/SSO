#include "header.h"

/*
	ARGV:	1: typ komunikatu (0: wszystkie, >0 konkretny typ)
			2: liczba komunikatów
			3: numer kolejki
*/
int main(int argc,char **argv)
{
	if (argc < 4)
	{
		printf("Błąd!\n konsument [typ_komunikatu] [liczba komunikatów] [numer_kolejki]\n");
		return 1;
	}

	srand(time(NULL));
	int type = atoi(argv[1]);
	int n = atoi(argv[2]);
	int msgqid = atoi(argv[3]);
	int rc;

	for (int i = 0; i < n; i++)
	{
		// Odczytanie komunikatu z kolejki
		rc = msgrcv(msgqid, &msg, sizeof(msg.mtext), type, 0); 
		if (rc < 0)
		{
			perror(strerror(errno));
			printf("Błąd odczytu %d\n", rc);
			return 1;
		} 
		printf("Odebrano [%ld] \"%s\"\n", msg.mtype, msg.mtext);
	}
	
	// Usunięcie kolejki
	rc=msgctl(msgqid,IPC_RMID,NULL);
	if (rc < 0)
	{
		perror( strerror(errno) );
		printf("Błąd usuwania kolejki%d\n", rc);
		return 1;
	}
	printf("________________________________________\nKolejka %d usunięta\n",msgqid);

	return 0;
}