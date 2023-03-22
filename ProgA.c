#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MSG_SIZE 256

// Define a message structure
struct message 
{
    long mtype;
    char mtext[MSG_SIZE];
};

int main() 
{
    int msqid;
    key_t key;
    struct message msg;

    // Generate a unique key
    key = ftok("progA.c", 'A');

    // Create a message queue
    msqid = msgget(key, 0666 | IPC_CREAT);
    if (msqid == -1) 
    {
        perror("msgget");
        exit(EXIT_FAILURE);
    }

    while (1) 
    {
        // Read a message from the terminal
        printf("Proga: ");
        fgets(msg.mtext, MSG_SIZE, stdin);

        // Send the message to the message queue
        msg.mtype = 1;
        if (msgsnd(msqid, &msg, strlen(msg.mtext) + 1, 0) == -1) 
        {
            perror("msgsnd");
            exit(EXIT_FAILURE);
        }

        // Terminate if the message is "Bye"
        if (strcmp(msg.mtext, "Bye\n") == 0) 
        {
            break;
        }

        // Receive a message from the message queue
        if (msgrcv(msqid, &msg, MSG_SIZE, 2, 0) == -1) 
        {
            perror("msgrcv");
            exit(EXIT_FAILURE);
        }

        // Print the received message
        printf("From ProgB: %s", msg.mtext);

        // Terminate if the message is "Bye"
        if (strcmp(msg.mtext, "Bye\n") == 0) 
        {
            break;
        }
    }

    // Remove the message queue
    if (msgctl(msqid, IPC_RMID, NULL) == -1) 
    {
        perror("msgctl");
        exit(EXIT_FAILURE);
    }

    return 0;
}
