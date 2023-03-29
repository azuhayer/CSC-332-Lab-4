#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/msg.h>

#define MSG_SIZE 1024

typedef struct {
    long mtype;
    char mtext[MSG_SIZE];
} msgbuf;

int main() {
    int msqid;
    key_t key;
    msgbuf msg;
    char input[MSG_SIZE];

    // Create a unique key for the message queue
    key = ftok("progA.c", 'A');
    if (key == -1) {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    // Create the message queue
    msqid = msgget(key, 0644 | IPC_CREAT);
    if (msqid == -1) {
        perror("msgget");
        exit(EXIT_FAILURE);
    }

    printf("Welcome to the chat program!\n");

    // Chat loop
    while (1) {
        // Send message to progB
        printf("Proga: ");
        fgets(input, MSG_SIZE, stdin);
        strcpy(msg.mtext, input);
        msg.mtype = 1;
        if (msgsnd(msqid, &msg, strlen(msg.mtext)+1, 0) == -1) {
            perror("msgsnd");
            exit(EXIT_FAILURE);
        }

        // Receive message from progB
        if (msgrcv(msqid, &msg, MSG_SIZE, 2, 0) == -1) {
            perror("msgrcv");
            exit(EXIT_FAILURE);
        }
        printf("From ProgB: %s", msg.mtext);

        // Check for termination signal
        if (strcmp(msg.mtext, "Bye\n") == 0) {
            break;
        }
    }

    // Remove message queue
    if (msgctl(msqid, IPC_RMID, NULL) == -1) {
        perror("msgctl");
        exit(EXIT_FAILURE);
    }

    printf("Closing chat program.\n");

    return 0;
}
