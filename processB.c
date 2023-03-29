#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SHM_SIZE 5

int main() {
    int shmid;
    key_t key;
    char *shm, c;

    // Generate a unique key for the shared memory segment
    key = ftok("shmfile", 'R');
    if (key == -1) {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    // Get the shared memory segment
    shmid = shmget(key, SHM_SIZE, 0644);
    if (shmid == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    // Attach the shared memory segment
    shm = (char *)shmat(shmid, NULL, 0);
    if (shm == (char *)-1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    // Change the first character to B
    *shm = 'B';
    printf("Process B: Wrote %.*s to shared memory.\n", SHM_SIZE, shm);

    // Wait until the first character is changed to C by process C
    while (*shm != 'C') {
        sleep(1);
    }

    // Print the updated shared memory and terminate
    printf("Process B: Read %.*s from shared memory.\n", SHM_SIZE, shm);

    // Detach the shared memory segment
    if (shmdt(shm) == -1) {
        perror("shmdt");
        exit(EXIT_FAILURE);
    }

    return 0;
}

