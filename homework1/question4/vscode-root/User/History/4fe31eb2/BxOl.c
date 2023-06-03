#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "rwbuf.h"

int main()
{
    int fd = open("/dev/rwbuf", O_RDWR);
    if(fd == -1)
    {
        perror(strerror(errno));
        return -1;
    }
    char buf[1025];
    memset(buf, 0, 1025);

    /*read test*/
    read(fd, buf, 3);
    printf("%s\n", buf);

    /*write test*/
    strcpy(buf, "176");
    write(fd, buf, 3);
    memset(buf, 0, 1025);
    read(fd, buf, 3);
    printf("%s\n", buf);

    /*ioctl read test*/
    memset(buf, 0, 1025);
    ioctl(fd, RW_READ, buf);
    printf("%s\n", buf);

    /*ioctl read old data test*/
    memset(buf, 0, 1025);
    ioctl(fd, RW_READ_OLD, buf);
    printf("%s\n", buf);

    /*ioctl clear test*/
    ioctl(fd, RW_CLEAR);
    read(fd, buf, 3);
    printf("%s\n", buf);

    /*mmap test*/
    memset(buf, 0, 1025);
    char* mmap_buf = mmap(NULL, 1025, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    /*mmap read test*/
    printf("%s\n", mmap_buf);
    /*mmap write test*/
    strcpy(mmap_buf, "177");
    msync(mmap_buf, 1025, MS_ASYNC);
    munmap(mmap_buf, 1025);
    read(fd, buf, 3);
    printf("%s\n", buf);

    /*test finish*/
    close(fd);    
    return 0;
}
