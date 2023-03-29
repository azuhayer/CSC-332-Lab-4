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

    // Create the shared memory segment
    shmid = shmget(key, SHM_SIZE, 0644 | IPC_CREAT);
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

    // Process A writes {A,A,A,A,A} to shared memory
    for (int i = 0; i < SHM_SIZE; i++) {
        *(shm + i) = 'A';
    }

    printf("Process A: Wrote %.*s to shared memory.\n", SHM_SIZE, shm);

    // Wait until the first character is changed to B by process B
    while (*shm != 'B') {
        sleep(1);
    }

    // Print the updated shared memory and terminate
    printf("Process A: Read %.*s from shared memory.\n", SHM_SIZE, shm);

    // Detach the shared memory segment
    if (shmdt(shm) == -1) {
        perror("shmdt");
        exit(EXIT_FAILURE);
    }

    return 0;
}

