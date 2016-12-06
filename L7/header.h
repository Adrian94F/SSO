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
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#define KROKI 10
#define SRV_IP "127.0.0.1"
#define SIZE 512
#define PORT 9999
#define SIZE 512
#define SERVPORT 8000
#define SERVADDR "127.0.0.1"

typedef enum 
{
	OPENR,
	READ,
	SHUTDOWN,
	SAVE, 
	CLOSE
} type;

typedef struct 
{
	type typ;
	char buf[SIZE];
	int handle;
	int ile;
} msg_t;

typedef enum { false, true } bool;