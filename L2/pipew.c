#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

#define LEN 65536
int main()
{
    int fd, i;
    char * myfifo = "kolejka";
    mkfifo(myfifo, 0666);
    fd = open(myfifo, O_WRONLY);
    for (i=0; i<LEN; i++)
    {
		write(fd, &i, sizeof(i));
		printf("Wysłano %d \n", i);
    }
    close(fd);
    unlink(myfifo);
    return 0;
}