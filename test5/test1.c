#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>


int main()
{
    int fd = open("./text.txt", O_RDWR | O_CREAT);
    if(fd == -1)
    {
        perror(strerror(errno));
        printf("open failed!\n");
        return -1;
    }
    if(write(fd, "21049200175", 11) == -1)
    {
        printf("write failed!\n");
        return -1;
    }
    close(fd);
    fd = open("./text.txt", O_RDWR | O_CREAT);
    if(fd == -1)
    {
        perror(strerror(errno));
        printf("open failed!\n");
        return -1;
    }
    if(lseek(fd, 5, SEEK_SET) == -1)
    {
        printf("lseek failed!\n");
        return -1;
    }
    char buf[100];
    if(read(fd, buf, 6) == -1)
    {
        printf("read failed!\n");
        return -1;
    }
    printf("%s\n", buf);
    close(fd);
    return 0;
}