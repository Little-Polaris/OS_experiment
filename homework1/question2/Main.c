#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

int file_op(char*, char);

int main(int argc, char* argv[])
{
    char dir[100];
    strcpy(dir, argv[1]);
    char op = argv[2][0];
    file_op(dir, op);
    return 0;
}

int file_op(char* dir, char op)
{
    int fd = 0;
    if(op == 'w')
    {
        fd = open(dir, O_WRONLY | O_CREAT, 0666);
        if(fd == -1)
        {
            printf("OPEN ERROR!\n");
            return -1;
        }
        char str[] = "175 MYFILE";
        if(write(fd, str, strlen(str)) == -1)
        {
            printf("WRITE ERROR!\n");
            return -1;
        }
        close(fd);
        return strlen(str);
    }
    else if(op == 'r')
    {
        fd = open(dir, O_RDONLY);
        if(fd == -1)
        {
            printf("OPEN ERROR!\n");
            return -1;
        }
        char* str = (char*)malloc(sizeof(char) * 1025);
        memset(str, 0, 1025);
        if(read(fd, str, 1024) == -1)
        {
            printf("READ ERROR!\n");
            return -1;
        }
        printf("Content of the file: \n%s\n", str);
        close(fd);
        return strlen(str);
    }
    else
    {
        printf("WRONG OPERATION!\n");
        return -1;
    }
}