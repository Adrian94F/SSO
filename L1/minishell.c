 #include <stdio.h> 
 #include <unistd.h> 
 #include <stdlib.h> 
 #include <sys/types.h> 
 #include <sys/stat.h> 
 #include <errno.h> 
 #include <fcntl.h> 

int main(int argc, char** argv)
{
	int status, pid, n, fd[2], lsout = 1;
	if (argc == 3)
	{
		pipe(fd);
		pid = fork();
		switch (pid)
		{
			case -1:
				perror("Nie udało się utworzyć procesu!");
				break;
			case 0:
				//dziecko
				dup2(fd[1], 1);
    	    	close(fd[0]);
    	    	execlp(argv[1], argv[1], NULL);
    	    	close(fd[1]);
				exit(0);
				break;
			default:
				//rodzic
				dup2(fd[0], 0);
    	        close(fd[1]);
				execlp(argv[2], argv[2], NULL);
				pid = wait(&status);
				close(fd[0]);
				break;
		}
	}
	return 0;
}
