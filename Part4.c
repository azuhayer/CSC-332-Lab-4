#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>

#define SHM_SIZE 5

int main() {
    key_t key = ftok("shared_memory", 1);
    int shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0666);
    char *shmaddr = shmat(shmid, NULL, 0);

    pid_t pidA, pidB, pidC;
    pidA = fork();

    if (pidA < 0) {
        perror("Error: fork() process A failed");
        exit(1);
    }
    else if (pidA == 0) {
        // Process A writes to shared memory
        char message[] = "AAAAA";
        for (int i = 0; i < SHM_SIZE; i++) {
            shmaddr[i] = message[i];
        }
        while (shmaddr[0] != 'B') {
            sleep(1);
        }
        printf("Process A: %s\n", shmaddr);
        exit(0);
    }
    else {
        pidB = fork();
        if (pidB < 0) {
            perror("Error: fork() process B failed");
            exit(1);
        }
        else if (pidB == 0) {
            // Process B changes first character to B
            while (shmaddr[0] != 'A') {
                sleep(1);
            }
            shmaddr[0] = 'B';
            while (shmaddr[0] != 'C') {
                sleep(1);
            }
            printf("Process B: %s\n", shmaddr);
            exit(0);
        }
        else {
            pidC = fork();
            if (pidC < 0) {
                perror("Error: fork() process C failed");
                exit(1);
            }
            else if (pidC == 0) {
                // Process C changes first character to C and detaches shared memory
                while (shmaddr[0] != 'B') {
                    sleep(1);
                }
                shmaddr[0] = 'C';
                shmdt(shmaddr);
                printf("Process C detached shared memory\n");
                exit(0);
            }
            else {
                // Wait for all child processes to finish
                waitpid(pidA, NULL, 0);
                waitpid(pidB, NULL, 0);
                waitpid(pidC, NULL, 0);
                // Delete shared memory segment
                shmctl(shmid, IPC_RMID, NULL);
                printf("All processes finished\n");
                exit(0);
            }
        }
    }
}
