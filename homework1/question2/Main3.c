#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <errno.h>

#define PAGE_SIZE 4096
#define PAGEMAP_ENTRY_SIZE 8 

int file_op(char*, char, ...);

int main(int argc, char* argv[])
{
    char dir[100];
    strcpy(dir, argv[1]);
    pid_t child_pid;
    int process_count = 2;
    key_t key = ftok("K", 1);
    int shm_id = shmget(key, sizeof(int), IPC_CREAT | 0666);
    if(shm_id == -1)
    {
        printf("SHMGET FAILED!\n");
        return -1;
    }
    int* turn;
    turn = (int*)shmat(shm_id, NULL, 0);
    if(turn == (int*)-1)
    {
        printf("SHMAT FAILED!\n");
        return -1;
    }

    unsigned long virt_addr = (unsigned long)turn;
    unsigned long offset = virt_addr / PAGE_SIZE * sizeof(unsigned long);
    char pagemap_dir[100];
    int pid = getpid();
    sprintf(pagemap_dir, "/proc/%d/pagemap", pid);
    int pagemap_fd = open(pagemap_dir, O_RDONLY);
    if(pagemap_fd == -1)
    {
        printf("OPEN PAGEMAP FAILED!\n");
        return -1;
    }
    if(lseek(pagemap_fd, offset,SEEK_SET ) == -1)
    {
        printf("LSEEK FAILED!\n");
        return -1;
    }
    unsigned long pagemap_data;
    if(read(pagemap_fd, &pagemap_data, sizeof(unsigned long)) != sizeof(unsigned long))
    {
        printf("READ PAGEMAP FAILED!\n");
        return -1;
    }
    if((pagemap_data & (1UL << 55)) == (1UL << 55))
    {
        char clear_refs_dir[100];
        sprintf(clear_refs_dir, "/proc/%d/clear_refs", pid);
        int clear_refs_fd = open(clear_refs_dir, O_WRONLY);
        if(clear_refs_fd == -1)
        {
            printf("OPEN CLAER_REFS FAILED!\n");
            return -1;
        }
        write(clear_refs_fd, "4", 1);
        if(read(pagemap_fd, &pagemap_data, sizeof(unsigned long)) != sizeof(unsigned long))
        {
            printf("READ PAGEMAP FAILED!\n");
            return -1;
        }
        close(clear_refs_fd);
        close(pagemap_fd);
    }
    unsigned long pfn = pagemap_data & ((1UL << 55) - 1);
    unsigned long phys_addr = pfn * PAGE_SIZE + virt_addr % PAGE_SIZE;
    char virt_addr_str[100];
    char phys_addr_str[100];

    sprintf(virt_addr_str, "Virtual address of \"turn\": 0x%lx\n", virt_addr);
    sprintf(phys_addr_str, "Physical address of \"turn\": 0x%lx\n", phys_addr);
    file_op(dir, 'a', virt_addr_str);
    file_op(dir, 'a', phys_addr_str);
    printf("%s", virt_addr_str);
    printf("%s", phys_addr_str);
    
    *turn = 0;
    for(int i = 0; i < process_count; i++)
    {
        child_pid = fork();
        if(child_pid == -1)
        {
            printf("FORK FAILED!\n");
            exit(-1);
        }
        else if(child_pid == 0)
        {
            int times = 5;
            while(times--)
            {
                while(*turn != i)
                {
                    sleep(1);
                }
                char str[100] = { 0 };
                sprintf(str, "175 PROC%d MYFILE%d\n", i + 1, i + 1);
                if(file_op(dir, 'a', str) == -1)
                {
                    printf("WIRTE FAILED!\n");
                    exit(-1);
                }
                memset(str, 0, 100);
                if(file_op(dir, 'r') == -1)
                {
                    printf("READ FAILED!\n");
                    exit(-1);
                } 
                *turn = (*turn + 1) % process_count;
            }
            exit(0);
        }
    }
    if(child_pid > 0)
    {
        wait(NULL);
    }
    shmctl(shm_id, IPC_RMID, NULL);
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