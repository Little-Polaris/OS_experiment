#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>

int main()
{
    int fd = open("./data.txt", O_RDWR);
    char* buf = mmap(NULL, 6, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    strcpy(buf, "175");
    munmap(buf, 6);
    return 0;
}