#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    FILE *file;
    pid_t pid;
    int i, n;
    int turns = 2; // 进程轮换的次数

    file = fopen("data.txt", "w"); // 打开文件（写入模式）
    if (file == NULL) {
        perror("Failed to open file");
        return 1;
    }

    // 创建两个子进程
    for (i = 0; i < turns; i++) {
        pid = fork();
        if (pid < 0) {
            perror("Failed to fork");
            return 1;
        } else if (pid == 0) {
            // 子进程
            n = i + 1; // 子进程编号

            // 严格轮换算法实现互斥锁
            for (int j = 0; j < turns; j++) {
                while (1) {
                    // 检查是否轮到当前子进程
                    int value;
                    FILE *turn = fopen("turn.txt", "r");
                    fscanf(turn, "%d", &value);
                    fclose(turn);
                    if (value == n)
                        break;
                }

                // 进程写入文件
                fprintf(file, "Process %d\n", n);
                fflush(file);

                // 更新轮到下一个进程
                int next = (n % turns) + 1;
                FILE *turn = fopen("turn.txt", "w");
                fprintf(turn, "%d", next);
                fclose(turn);
            }

            return 0;
        }
    }

    // 父进程等待子进程结束
    for (i = 0; i < turns; i++) {
        wait(NULL);
    }

    // 关闭文件
    fclose(file);

    return 0;
}
