#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

int main() {
    int file_descriptor;
    FILE* file;
    int process_count = 2;
    int current_process = 0;
    pid_t child_pid;
    
    // 打开文件
    file_descriptor = open("example.txt", O_RDWR | O_CREAT, 0666);
    if (file_descriptor == -1) {
        perror("open");
        return 1;
    }
    
    // 将文件描述符转换为 FILE 结构体指针
    file = fdopen(file_descriptor, "w+");
    if (file == NULL) {
        perror("fdopen");
        close(file_descriptor);
        return 1;
    }
    
    // 创建子进程并使用严格轮换算法实现互斥锁
    for (int i = 0; i < process_count; i++) {
        child_pid = fork();
        
        if (child_pid < 0) {
            perror("fork");
            return 1;
        } else if (child_pid == 0) {
            // 子进程
            while (1) {
                // 检查是否轮到当前进程执行
                if (current_process == i) {
                    // 执行文件操作
                    fprintf(file, "Process %d is writing\n", i);
                    fflush(file);
                    
                    // 递增当前进程编号，实现轮换
                    current_process = (current_process + 1) % process_count;
                }
                
                // 等待其他进程执行完毕
                while (current_process != i) {
                    usleep(100);
                }
            }
            
            break;
        }
    }
    
    // 等待子进程结束
    if (child_pid > 0) {
        wait(NULL);
    }
    
    // 关闭文件和文件描述符
    fclose(file);
    close(file_descriptor);
    
    return 0;
}
