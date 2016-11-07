#include <string.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <sys/errno.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h> 
		 
extern int errno;			// error NO.
#define MSGPERM 0600		// msg queue permission
#define MSGTXTLEN 128		// msg text length

struct msg_buf {
	long mtype;
	char mtext[MSGTXTLEN];
} msg;