#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#define LEN 65536
int main()
{
    int fd, i, n;
    char * myfifo = "kolejka";
    fd = open(myfifo, O_RDONLY);
    for (i=0; i<LEN; i++)
    {
    	read(fd, &n, sizeof(n));
    	printf("Odebrano %d \n", n);
    	if (i%1000 == 0)
    		sleep(1);
    }
    close(fd);
    return 0;
}