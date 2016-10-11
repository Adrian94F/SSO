 #include <stdio.h> 
 #include <unistd.h> 
 #include <stdlib.h> 
 #include <sys/types.h> 
 #include <sys/stat.h> 
 #include <errno.h> 
 #include <fcntl.h> 

int msleep(unsigned long milisec) //sleep w milisekundach
{   
    struct timespec req={0};   
    time_t sec=(int)(milisec/1000);   
    milisec=milisec-(sec*1000);   
    req.tv_sec=sec;   
    req.tv_nsec=milisec*1000000L;   
    while(nanosleep(&req,&req)==-1)   
        continue;   
    return 1;   
} 

int main(int argc, char** argv)
{
	int kolejka, status, pid;

	mkfifo("kolejka", 0666);
	kolejka = open("kolejka", O_RDWR);

	if(pid = fork() == 0)
	{
		//dziecko
		int n;
		for (int i=0; i < 65536; i++)
		{
			if(read(kolejka, &n, sizeof(n)) < 0)
   				perror("Błąd odczytu");
   			printf("%d ", n);
   		}
		exit(0);
	}
	//rodzic
	for (int i=0; i < 65536; i++)
	{
		write(kolejka, &i, sizeof(i));
	}

	//czekanie
	pid = wait(&status);
	printf("Proces %d zakończony\n", pid);

	close(kolejka);
	return 0;
}