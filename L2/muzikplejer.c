#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

void splash()
{
	printf("*** MuzikPlejer ***\n  Sterowanie:\n   spacja  pauza\n   q       wyjście\n   -       przewiń w tył\n   =       przewiń w przód\n   [       zmniejsz prędkośc odtwarzania\n   ]       zwiększ prędkośc odtwarzania\n\n");
}


int main(int argc, char** argv)
{
	if (argc < 1)
	{
		printf("USAGE: muzikplejer file_path\n");
		return -1;
	}
	FILE* fifo;
	int enabled = 1, pid;
	char key = 0;
	char * myfifo = "./.p";
	mknod(myfifo, S_IFIFO | 0666, 0);

	pid = fork();
	switch (pid)
	{
		case -1:
			perror("BŁĄD BARDZO KRYTYCZNY - fork:");
			return -1;
		case 0:
			/* dziecko */
			/* przekierowanie wejścia i wyjść do /dev/null */
			freopen("/dev/null", "r", stdin);
			freopen("/dev/null", "w", stdout);
			freopen("/dev/null", "w", stderr);
			/* uruchomienie odtwarzacza */
			execlp("mplayer", "mplayer", "-input", "file=./.p", argv[1], "&", NULL);
			break;
		default:
			/* rodzic */
			splash();
			do
			{
				/* wymuszenie wysyłania przez terminal kazdego naciśnięcia klawisza do stdin 
				+ wyłączenie wyświetlania wciśniętych klawiszy */
				system("stty raw -echo");
				do
				{
					key = getchar();
				}
				while(key == '\n');
				/* powrót do normalnego trybu */
				system ("stty cooked");
				/* printf(" - %d\n", key); */
				
				fifo = fopen(myfifo, "w");
				switch(key)
				{
					case 'q':
						fprintf(fifo, "quit\n" );
						printf("Koniec!\n");
						enabled = 0;
						break;
					case 32:
						fprintf(fifo, "pause\n");
						break;
					case '[':
						fprintf(fifo, "speed_incr -0.25\n");
						break;
					case ']':
						fprintf(fifo, "speed_incr 0.25\n");
						break;
					case '-':
						fprintf(fifo, "seek -10\n");
						break;
					case '=':
						fprintf(fifo, "seek 10\n");
						break;
					default:
						/* printf("Wrong command %c\n", key); */
						break;
				}
				fclose(fifo);
			}
			while(enabled);
	}
	return 0;
}