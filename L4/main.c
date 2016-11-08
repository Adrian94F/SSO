/*
	5 filozofów - procesów
	wersja z bramkarzem (wpuszczamy 4 do stołu)
*/
#include <semaphore.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <time.h>

#define clear() printf("\033[H\033[J")
#define PHILOS 3	// Liczba filozofow

static volatile int keepRunning = 1;

void intHandler(int dummy) 
{
    keepRunning = 0;
}

void msleep (unsigned int ms) {
    int microsecs;
    struct timeval tv;
    microsecs = ms * 1000;
    tv.tv_sec  = microsecs / 1000000;
    tv.tv_usec = microsecs % 1000000;
    select (0, NULL, NULL, NULL, &tv);  
}


typedef struct
{
	char chairs[PHILOS];
	char forks[PHILOS];
	sem_t waiterSem;
	sem_t forksSem[PHILOS];
}shmstruct;


int main()
{
	int pid;

	key_t key;
	int shmd;

	signal(SIGINT, intHandler);
	// Pamięć współdzielona
	key = ftok("mem", 0);
	shmd = shmget(key, sizeof(shmstruct), 0777 | IPC_CREAT);
	shmstruct *data = shmat(shmd, (void *)0, 0);
	if (data == NULL)
    	perror("shmat");

    // Semafory i czyszczenie
    if(sem_init(&(data->waiterSem), 1, PHILOS - 1))
	{ 
		perror("mutex");
		exit(0);  
	}
	for (int i = 0; i < PHILOS; i++)
	{
    	if(sem_init(&(data->forksSem[i]), 1, 1))
		{ 
			perror("mutex");
			exit(0);  
		}
		data->chairs[i] = ' ';
		data->forks[i] = ' ';
	}

	// Tworzenie dzieci
	for (int i = 0; i < PHILOS; i++)
	{
		pid = fork();
		switch(pid)
		{
			case -1:
				// Błąd
				break;
			case 0:
				// Dzecko
				srand(time(NULL) ^ getpid() << 16);
				while(keepRunning)
				{
					// Filozof próbuje wejść
					sem_wait(&(data->waiterSem));
					// Próbuje wziąć widelec z prawej
					int left = (i+1)%PHILOS;
					int right = i;
					sem_wait(&(data->forksSem[left]));
					data->chairs[i] = '-';
					data->forks[left] = 'l';
					// Próbuje wziać z lewej
					sem_wait(&(data->forksSem[right]));
					data->forks[right] = 'r';
					// Je
					data->chairs[i] = 'V';
					msleep(100+rand()%2000);
					// Odkłada widelce
					data->forks[left] = ' ';
					sem_post(&(data->forksSem[left]));
					data->forks[right] = ' ';
					sem_post(&(data->forksSem[right]));
					// Wstaje
					data->chairs[i] = ' ';
					sem_post(&(data->waiterSem));
					msleep(2000+rand()%5000);
				}
				break;
			default:
				// Rodzic
				break;
		}
	}
	while(keepRunning)
	{
		clear();
		printf("Widelce ");
		for (int i = 0; i < PHILOS; i++)
			printf("[ %c ] ", data->forks[i]);
		printf("\nMiejsca    ");
		for (int i = 0; i < PHILOS; i++)
			printf("[ %c ] ", data->chairs[i]);
		printf("\n");
		msleep(5);
	}
	shmdt(data);
	shmctl(shmd, IPC_RMID, NULL);
	exit(0);
}