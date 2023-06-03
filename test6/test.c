#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>

#define PAGE_SIZE 4096
#define PAGE_SHIFT 12

int main() {
    pid_t pid = getpid();
    char pagemap_file[64];
    sprintf(pagemap_file, "/proc/%d/pagemap", pid);
    int data = 1;

    int fd = open(pagemap_file, O_RDONLY);
    if (fd < 0) {
        perror("Failed to open pagemap file");
        exit(1);
    }

    // 获取变量的物理地址
    uintptr_t virtual_address = (uintptr_t)&data;
    off_t offset = (virtual_address / PAGE_SIZE) * sizeof(uint64_t);

    uint64_t pte;
    if (pread(fd, &pte, sizeof(uint64_t), offset) != sizeof(uint64_t)) {
        perror("Failed to read PTE");
        exit(1);
    }

    uint64_t pfn = pte & ((1ULL << 55) - 1);  // 获取物理页框号

    uintptr_t physical_address = (pfn << PAGE_SHIFT) + (virtual_address & (PAGE_SIZE - 1));
    
    printf("Variable's physical address: 0x%lx\n", physical_address);

    close(fd);
    return 0;
}
