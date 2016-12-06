#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <mqueue.h>
#include <semaphore.h>

#define BSIZE 4		//rozmiar bufora
#define LSIZE 256	//długość linii

typedef struct
{
	char buffer[BSIZE][LSIZE];
	int head;
	int tail;
	int count;
	sem_t mutex;
	sem_t empty;
	sem_t full;
} bufor_t;