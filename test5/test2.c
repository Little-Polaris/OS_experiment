#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
    int fd = open("./text.txt", O_RDWR | O_CREAT);
    if(fd == -1)
    {
        printf("open failed!\n");
        return -1;
    }
   if(write(fd, "21049200176", 11)== -1)
   {
        printf("write failed!\n");
        return -1;
   }
    close(fd);
    return 0;
}