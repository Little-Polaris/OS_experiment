#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define NUM_PROCESSES 5

int main() {
    int i;
    pid_t pid;

    // 创建共享资源（文件、内存区域等）
    FILE *file = fopen("shared_resource.txt", "w");
    if (file == NULL) {
        perror("Error opening file");
        exit(1);
    }

    // 创建多个子进程
    for (i = 0; i < NUM_PROCESSES; i++) {
        pid = fork();
        if (pid < 0) {
            perror("Error forking process");
            exit(1);
        } else if (pid == 0) {
            // 子进程访问共享资源
            printf("Process %d: Entered critical section\n", getpid());

            // 访问共享资源的代码
            fprintf(file, "Process %d wrote to the file\n", getpid());
            fflush(file);

            printf("Process %d: Exiting critical section\n", getpid());

            // 子进程退出
            exit(0);
        }
    }

    // 等待所有子进程结束
    for (i = 0; i < NUM_PROCESSES; i++) {
        wait(NULL);
    }

    // 关闭共享资源
    fclose(file);

    return 0;
}
