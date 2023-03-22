#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MAX_TEXT 512

struct message {
    long type;
    char text[MAX_TEXT];
};

int main() {
    key_t key;
    int msgid;
    struct message msg;
    pid_t pid = getpid();

    // Generate a unique key for the message queue
    if ((key = ftok(".", 'a')) == -1) {
        perror("ftok");
        exit(1);
    }

    // Create the message queue with read-write permissions for the owner
    if ((msgid = msgget(key, 0644 | IPC_CREAT)) == -1) {
        perror("msgget");
        exit(1);
    }

    while (1) {
        // Read user input from the terminal and send it to the message queue
        printf("From Prog%d: ", pid);
        fgets(msg.text, MAX_TEXT, stdin);
        msg.type = pid;
        if (msgsnd(msgid, &msg, sizeof(msg.text), 0) == -1) {
            perror("msgsnd");
            exit(1);
        }

        // Terminate the program if the user types "Bye"
        if (strncmp(msg.text, "Bye", 3) == 0) {
            break;
        }

        // Wait for a message from the other program
        if (msgrcv(msgid, &msg, sizeof(msg.text), pid == 1 ? 2 : 1, 0) == -1) {
            perror("msgrcv");
            exit(1);
        }

        // Print the received message to the terminal
        printf("From Prog%d: %s", pid == 1 ? 2 : 1, msg.text);

        // Terminate the program if the other program types "Bye"
        if (strncmp(msg.text, "Bye", 3) == 0) {
            break;
        }
    }

    // Remove the message queue
    if (msgctl(msgid, IPC_RMID, NULL) == -1) {
        perror("msgctl");
        exit(1);
    }

    return 0;
}
