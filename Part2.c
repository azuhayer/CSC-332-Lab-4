#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void child_handler(int signum) 
{
    printf("Child received signal %d\n", signum);
    exit(0);
}

int main() 
{
    pid_t pid;
    
    pid = fork();

    if (pid < 0) 
    {
        perror("fork failed");
        exit(1);
    } 
    
    else if (pid == 0) 
    {
        // child process
        signal(SIGINT, child_handler);
        signal(SIGTERM, child_handler);
        printf("Child process is waiting for signals...\n");
        while(1) 
        {
            // wait for signals
        }
    } 
    else 
    {
        // parent process
        sleep(1);
        kill(pid, SIGINT);
        sleep(1);
        kill(pid, SIGTERM);
        printf("Parent process sent signals to child process\n");
        wait(NULL);
        printf("Child process terminated\n");
    }

    return 0;
}