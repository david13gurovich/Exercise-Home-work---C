#define _GNU_SOURCE
#include <stdio.h> 
#include <sys/ipc.h> 
#include <sys/msg.h> 
#include <stdlib.h>
#include <assert.h>
#include <string.h>

const int FAIL = -1;

typedef struct massage_s
{
    long type;
    char text[100];
} message_t;

int MassageQueueWriter()
{
    message_t message = {0};
    message_t message1 = {0};
    key_t key = ftok("./massage_queue.txt", 65);
    int msgid = msgget(key, 0666 | IPC_CREAT); 
    if(FAIL == msgid)
    {
        perror("");
        return (EXIT_FAILURE);
    }
    
    message.type = 1; 
    message1.type = 1; 
    strcpy(message.text, "message_queue - TEXT!");
    strcpy(message1.text, "message_queue - SECOND TEXT!");

    msgsnd(msgid, &message, sizeof(message_t), 0);
    msgsnd(msgid, &message1, sizeof(message_t), 0);
    printf("Data send is : %s \n", message.text);
    printf("Data send is : %s \n", message1.text);

    return (EXIT_SUCCESS);
}

int main()
{
    if (EXIT_FAILURE == MassageQueueWriter())
    {
        return (EXIT_FAILURE);
    }
    return (EXIT_SUCCESS);
}