#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>

// 定义信号量操作结构体
struct sembuf acquire_op = {0, -1, SEM_UNDO};
struct sembuf release_op = {0, 1, SEM_UNDO};

int main() {
    // 创建信号量集合
    int sem_id = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
    if (sem_id == -1) {
        perror("创建信号量集合失败");
        exit(1);
    }

    // 初始化信号量
    if (semctl(sem_id, 0, SETVAL, 1) == -1) {
        perror("初始化信号量失败");
        exit(1);
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("创建子进程失败");
        exit(1);
    } else if (pid == 0) {
        // 子进程
        // 获取信号量锁
        if (semop(sem_id, &acquire_op, 1) == -1) {
            perror("获取信号量锁失败");
            exit(1);
        }

        // 文件操作
        FILE *file = fopen("file.txt", "a");
        if (file == NULL) {
            perror("打开文件失败");
            exit(1);
        }

        fprintf(file, "Hello from child process\n");

        fclose(file);

        // 释放信号量锁
        if (semop(sem_id, &release_op, 1) == -1) {
            perror("释放信号量锁失败");
            exit(1);
        }

        exit(0);
    } else {
        // 父进程
        // 获取信号量锁
        if (semop(sem_id, &acquire_op, 1) == -1) {
            perror("获取信号量锁失败");
            exit(1);
        }

        // 文件操作
        FILE *file = fopen("file.txt", "a");
        if (file == NULL) {
            perror("打开文件失败");
            exit(1);
        }

        fprintf(file, "Hello from parent process\n");

        fclose(file);

        // 释放信号量锁
        if (semop(sem_id, &release_op, 1) == -1) {
            perror("释放信号量锁失败");
            exit(1);
        }

        // 等待子进程结束
        wait(NULL);
    }

    // 删除信号量集合
    if (semctl(sem_id, 0, IPC_RMID) == -1) {
        perror("删除信号量集合失败");
        exit(1);
    }

    return 0;
}
