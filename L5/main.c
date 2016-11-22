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
#define PHILOS 5		// Liczba filozofow
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
	char status[PHILOS];
	int meals[PHILOS];
	int tries[PHILOS];
	sem_t waiterSem;
	sem_t forksSem[PHILOS];
} shmstruct;
shmstruct *data;
static volatile int keepRunning = 1;
int pid[PHILOS];
key_t key;
int shmd;
void parentHandler(int dummy) 
{
	printf("\nPrzechwycono sygnał! Zabijcie się!\n");
	keepRunning = 0;
	for (int i = 0; i < PHILOS; i++)
		kill(pid[i], SIGABRT);
	msleep(1000);
	shmdt(data);
	shmctl(shmd, IPC_RMID, NULL);
	exit(0);
}
void childIntHandler(int dummy)
{
	return;
}
void childHandler(int dummy) 
{
	printf("Żegnaj świecie!\n");
	keepRunning = 0;
	shmdt(data);
	shmctl(shmd, IPC_RMID, NULL);
	exit(0);
}
void printHandler(int dummy)//sigusr1
{
	clear();
	for (int i = 0; i < PHILOS; i++)
		printf("[ %c ] ", data->forks[i]);
	printf("\n     Miejsca    ");
	for (int i = 0; i < PHILOS; i++)
		printf("[ %c ] ", data->chairs[i]);
	printf("\n\n Status filozofów (_ - myśli, w - czeka, l - lewy, r - prawy, e - je)\n");
	for (int i = 0; i < PHILOS; i++)
	{
		printf("     F. %d (stat. %c) jadł %d razy, nie jadł %d razy\n", i, data->status[i], data->meals[i], data->tries[i]);
	}
}
void init()
{
// Pamięć współdzielona
	key = ftok("mem", 0);
	shmd = shmget(key, sizeof(shmstruct), 0777 | IPC_CREAT);
	data = shmat(shmd, (void *)0, 0);
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
		data->status[i] = ' ';
		data->meals[i] = 0;
	}
}
int main()
{
	init();
	// Tworzenie dzieci
	for (int i = 0; i < PHILOS; i++)
	{
		pid[i] = fork();
		switch(pid[i])
		{
			case -1:
				// Błąd
				exit(1);
			case 0:
				// Dzecko
				printf("%d --> %d\n", getppid(), getpid());
				signal(SIGABRT, childHandler);
				signal(SIGINT, childIntHandler);
				srand(time(NULL) ^ getpid() << 16);
				int left = i;
				int right = (i+1)%PHILOS;
				while(keepRunning)
				{
					// Filozof próbuje wejść
					data->status[i] = 'w';
					if (sem_trywait(&(data->waiterSem)) == 0)
					{
						// Próbuje wziąć widelec z lewej
						data->status[i] = 'l';
						if (sem_trywait(&(data->forksSem[left])) == 0)
						{
							data->chairs[i] = '-';
							data->forks[left] = 'l';
							// Próbuje wziać z prawej
							data->status[i] = 'r';
							if (sem_wait(&(data->forksSem[right])) == 0)
							{
								data->forks[right] = 'r';
								// Je
								data->status[i] = 'e';
								data->chairs[i] = 'V';
								data->meals[i]++;
								msleep(100+rand()%2000);
								// Odkłada widelce
								data->forks[left] = ' ';
								sem_post(&(data->forksSem[left]));
							}
							else
								data->tries[i]++;
							data->forks[right] = ' ';
							sem_post(&(data->forksSem[right]));
						}
						else
							data->tries[i]++;
						// Wstaje
						data->chairs[i] = ' ';
						data->status[i] = ' ';
						sem_post(&(data->waiterSem));
					}
					else
						data->tries[i]++;
					msleep(1000+rand()%3000);
				}
				exit(0);
				break;
			default:
				// Rodzic
				break;
		}
	}
	// Rodzica ciąg dalszy	
	signal(SIGINT, parentHandler);
	signal(SIGABRT, parentHandler);
	signal(SIGUSR1, printHandler);
	while(keepRunning)
	{
		msleep(1000);
	}
	exit(0);
}