#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <asm/unistd.h>

int file_op(char*, char, ...);

int main(int argc, char* argv[])
{
    char dir[100];
    strcpy(dir, argv[1]);
    int shm_id = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666);
    if(shm_id == -1)
    {
        printf("SHMGET FAILED!\n");
        return -1;
    }
    int* lock = (int*)shmat(shm_id, NULL, 0);
    if(lock == (int*)-1)
    {
        printf("SHMAT FAILED!\n");
        return -1;
    }
    *lock = 1;
    pid_t pid = fork();
    if(pid == -1)
    {
        printf("FORK FAILED!\n");
        return -1;
    }
    else if(pid == 0)
    {
        int c_times = 5;
        while(c_times--)
        {
            asm volatile
            (
                "movl $0, %%eax\n"
                "xchg %%eax, %0\n"
                "lp_lc1:\n"
                "cmp $0, %%eax\n"
                "je lp_lc1\n"
                :"=m"(*lock)
                :
                :"%eax"
            );
            char str[] = "175 PROC2 MYFILE2\n";
            if(file_op(dir, 'a', str) == -1)
            {
                printf("FILE OPERATION FAILED!\n");
                exit(-1);
            }
            if(file_op(dir, 'r') == -1)
            {
                printf("READ FIALED!\n");
                exit(-1);
            }
            asm volatile
            (
                "movl $1, %%eax\n"
                "xchg %%eax, %0\n"
                :"=m"(*lock)
                :
                :"%eax"
            );
            sleep(1);
        }
        exit(0);
    }
    else
    {
        int c_times = 5;
        while(c_times--)
        {
            asm volatile
            (
                "movl $0, %%eax\n"
                "xchg %%eax, %0\n"
                "lp_lc2:\n"
                "cmp $0, %%eax\n"
                "je lp_lc2\n"
                :"=m"(*lock)
                :
                :"%eax"
            );
            char str[] = "175 PROC1 MYFILE1\n";
            if(file_op(dir, 'a', str) == -1)
            {
                printf("FILE OPERATION FAILED!\n");
                exit(-1);
            }
            if(file_op(dir, 'r') == -1)
            {
                printf("READ FIALED!\n");
                exit(-1);
            }
            asm volatile
            (
                "movl $1, %%eax\n"
                "xchg %%eax, %0\n"
                :"=m"(*lock)
                :
                :"%eax"
            );
            sleep(1);
        }
        exit(0);
    }
    return 0;
}

int file_op(char* dir, char op, ...)
{
    int fd = 0;
    if(op == 'w')
    {
        va_list args;
        va_start(args, op);
        char* str = va_arg(args, char*);
        va_end(args);
        fd = open(dir, O_CREAT | O_WRONLY, 0666);
        if(fd == -1)
        {
            printf("OPEN ERROR!\n");
            return -1;
        }
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
        fd = open(dir, O_RDONLY, 0666);
        if(fd == -1)
        {
            printf("OPEN ERROR!\n");
            return -1;
        }
        char str[1025];
        memset(str, 0, 1025);
        if(read(fd, str, 1024) == -1)
        {
            printf("READ ERROR!\n");
            return -1;
        }
        printf("Content of the file: \n%s\n\n", str);
        close(fd);
        return strlen(str);
    }
    else if(op == 'a')
    {
        va_list args;
        va_start(args, op);
        char* str = va_arg(args, char*);
        va_end(args);
        fd = open(dir, O_CREAT | O_WRONLY | O_APPEND, 0666);
        if(fd == -1)
        {
            printf("OPEN ERROR!\n");
            return -1;
        }
        if(write(fd, str, strlen(str)) == -1)
        {
            printf("WRITE ERROR!\n");
            return -1;
        }
        close(fd);
        return strlen(str);
    }
    else
    {
        printf("WRONG OPERATION!\n");
        return -1;
    }
}