#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char** argv)
{
	FILE* fifo;
	int enabled = 1, pid, sid;
	char key = 0;
	char * myfifo = "./kolejka";
	mknod(myfifo, S_IFIFO | 0666, 0);
	perror("mknod");

	pid = fork();
	switch (pid)
	{
		case -1:
			perror("BŁĄD BARDZO KRYTYCZNY - fork:");
			return -1;
		case 0:
			/* dziecko */
			/* Change the file mode mask */
    		umask(0);
    		/* chdir("/"); */
    		/* Create a new SID for the child process */
    		sid = setsid();
    		if (sid < 0) {
    		    exit(-1);
    		}
    		/* Redirect standard files to /dev/null */
    		freopen("/dev/null", "r", stdin);
    		freopen("/dev/null", "w", stdout);
    		freopen("/dev/null", "w", stderr);
			execlp("mplayer", "mplayer", "-noconsolecontrols", "-input", "file=kolejka", "Layla.mp3", "&", NULL);
			break;
		default:
			/* rodzic */
			do
			{
				/* wymuszenie wysyłania przez terminal kazdego naciśnięcia klawisza do stdin */
				system("stty raw");
				do
				{
					key = getchar();
				}
				while(key == '\n');
				/* powrót do normalnego trybu */
				system ("stty cooked");
				printf(" - %d\n", key);
				
				fifo = fopen(myfifo, "w");
				perror("fopen");
				switch(key)
				{
					case 'q':
						fprintf(fifo, "quit\n" );
						printf("End!\n");
						enabled = 0;
						break;
					/*case 27:
						fprintf(fifo, "quit\n" );
						printf("End!\n");
						enabled = 0;
						break;*/
					case 'p':
						fprintf(fifo, "pause\n");
						break;
					case 'a':
						fprintf(fifo, "speed_incr -0.25\n");
						break;
					case 'd':
						fprintf(fifo, "speed_incr 0.25\n");
						break;
					case 'z':
						fprintf(fifo, "seek -10\n");
						break;
					case 'x':
						fprintf(fifo, "seek 10\n");
						break;
					default:
						printf("Wrong command %c (%d)\n", key, key);
						break;
				}
				fclose(fifo);
			}
			while(enabled);
	}
	return 0;
}