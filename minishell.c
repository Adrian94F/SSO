 #include <stdio.h> 
 #include <unistd.h> 
 #include <stdlib.h> 
 #include <sys/types.h> 
 #include <sys/stat.h> 
 #include <errno.h> 
 #include <fcntl.h> 

int main(int argc, char** argv)
{
	int kolejka, status, pid, n;

	mkfifo("kolejka", 0666);
	kolejka = open("kolejka", O_RDWR);

	pid = fork();
	switch (pid)
	{
		case -1:
			perror("Nie udało się utworzyć procesu!");
			break;
		case 0:
			//dziecko
			execlp("ls", "ls", "-n", NULL);
			//for (int i=0; i < 65536; i++)
			//{
			//	if(read(kolejka, &n, sizeof(n)) < 0)
   			//		perror("Błąd odczytu");
   			//	printf("%d ", n);
   			//}
			exit(0);
			break;
		default:
			//rodzic
			execlp("cat", "cat", "-n", NULL);
			//for (int i=0; i < 65536; i++)
			//{
			//	write(kolejka, &i, sizeof(i));
			//}
			//czekanie
			pid = wait(&status);
			printf("\nProces dziecko %d zakończony\n", pid);
			close(kolejka);
			break;
	}
	return 0;
}