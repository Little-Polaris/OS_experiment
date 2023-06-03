#include <stdio.h>
#include <sys/syscall.h>
#include <linux/kernel.h>
#include <unistd.h>

int main()
{
    int answer = syscall(548, -3, 0);
    printf("%d\n", answer);
    return 0;
}