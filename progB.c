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

    // Get the message queue
    msqid = msgget(key, 0644);
    if (msqid == -1) {
        perror("msgget");
        exit(EXIT_FAILURE);
    }

    printf("Welcome to the chat program!\n");

    // Chat loop
    while (1) {
        // Receive message from progA
        if (msgrcv(msqid, &msg, MSG_SIZE, 1, 0) == -1) {
            perror("msgrcv");
            exit(EXIT_FAILURE);
        }
        printf("From ProgA: %s", msg.mtext);

        // Check for termination signal
        if (strcmp(msg.mtext, "Bye\n") == 0) {
            break;
        }

        // Send message to progA
        printf("Progb: ");
        fgets(input, MSG_SIZE, stdin);
        strcpy(msg.mtext, input);
        msg.mtype = 2;
        if (msgsnd(msqid, &msg, strlen(msg.mtext)+1, 0) == -1) {
            perror("msgsnd");
            exit(EXIT_FAILURE);
        }
    }

    printf("Closing chat program.\n");

    return 0;
}
