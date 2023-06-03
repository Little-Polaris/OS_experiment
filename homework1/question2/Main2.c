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

struct sembuf acquire_op = {0, -1, SEM_UNDO};
struct sembuf release_op = {0, 1, SEM_UNDO};

int file_op(char*, char, ...);

int main(int argc, char* argv[])
{
    char dir[100];
    strcpy(dir, argv[1]);
    int sem_id = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
    if(sem_id == -1)
    {
        printf("CREATATION FAILED!\n");
        return -1;
    }
    if(semctl(sem_id, 0, SETVAL, 1) == -1)
    {
        printf("INITIALIZATION FAILED!\n");
        return -1;
    }
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
            if(semop(sem_id, &acquire_op, 1) == -1)
            {
                printf("ACQUIRATION FAILED!\n");
                exit(-1);
            }
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
            if(semop(sem_id, &release_op, 1) == -1)
            {
                printf("RELEASE FAILED!\n");
                exit(-1);
            }
            sleep(1);
        }
        exit(0);
    }
    else
    {
        int c_times = 5;
        while(c_times--)
        {
            if(semop(sem_id, &acquire_op, 1) == -1)
            {
                printf("ACQUIRATION FAILED!\n");
                exit(-1);
            }
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
            if(semop(sem_id, &release_op, 1) == -1)
            {
                printf("RELEASE FAILED!\n");
                exit(-1);
            }
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