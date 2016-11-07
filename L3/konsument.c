#include "header.h"

int msgqid, rc;
int done;

int main(int argc,char **argv)
{
	// Utworzenie kolejki
	msgqid = msgget(IPC_PRIVATE, MSGPERM|IPC_CREAT|IPC_EXCL);
	if (msgqid < 0) {
		perror(strerror(errno));
		printf("Błąd tworzenia kolejki o msgqid = %d\n", msgqid);
		return 1;
	}
	printf("Kolejka %d utworzona\n",msgqid);
	
	// Wiadomość do wysłania
	msg.mtype = 1; 										// Ustawienie typu wiadomości
	sprintf (msg.mtext, "%s\n", "wiadomość do wysłania");

	// Wysłanie wiadomości do kolejki
	rc = msgsnd(msgqid, &msg, sizeof(msg.mtext), 0);	// Ostatni parametr: 0, IPC_NOWAIT, MSG_NOERROR lub IPC_NOWAIT|MSG_NOERROR
	if (rc < 0) {
		perror( strerror(errno) );
		printf("Błąd wysyłania %d\n", rc);
		return 1;
	}

	// Odczytanie wiadomości z kolejki
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