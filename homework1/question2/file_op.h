int file_op(char* dir, char op, ...)
{
    int fd = 0;
    if(op ==     'w')
    {
        va_list args;
        va_start(args, op);
        char* str = va_arg(args, char*);
        va_end(args);
        fd = open(dir, O_WRONLY | O_CREAT, 0666);
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
        char* str = (char*)malloc(sizeof(char) * 1025);
        memset(str, 0, 1025);
        if(read(fd, str, 1024) == -1)
        {
            printf("READ ERROR!\n");
            return -1;
        }
        printf("Content of the file: %s\n", str);
        close(fd);
        return strlen(str);
    }
    else
    {
        printf("WRONG OPERATION!\n");
        return -1;
    }
}