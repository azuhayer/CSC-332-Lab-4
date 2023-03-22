#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main()
{
    int fd[2];
    pid_t pid;
    char buf[1024];

    if (pipe(fd) == -1) 
    {
        perror("pipe");
        exit(1);
    }

    pid = fork();

    if (pid < 0) 
    {
        perror("fork");
        exit(1);
    }

    if (pid == 0) 
    {  
        // child process
        FILE *fp;
        char *filename = "readme.txt";
        char filebuf[1024];

        fp = fopen(filename, "r");
        if (fp == NULL) 
        {
            perror("fopen");
            exit(1);
        }

        fread(filebuf, sizeof(char), 1024, fp);
        close(fd[0]);  // close read end of pipe
        write(fd[1], filebuf, strlen(filebuf) + 1);
        close(fd[1]);  // close write end of pipe
        fclose(fp);
        exit(0);

    } 
    else 
    {  
        // parent process
        wait(NULL);
        char writebuf[1024];
        sprintf(writebuf, "Parent is writing: %s", buf);
        FILE *fp;
        char *filename = "readme.txt";

        fp = fopen(filename, "w");
        if (fp == NULL) 
        {
            perror("fopen");
            exit(1);
        }

        close(fd[1]);  // close write end of pipe
        read(fd[0], buf, 1024);
        fprintf(fp, "%s", writebuf);
        close(fd[0]);  // close read end of pipe
        fclose(fp);
    }

    return 0;
}