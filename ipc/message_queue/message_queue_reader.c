#define _GNU_SOURCE
#include <stdio.h> 
#include <sys/ipc.h> 
#include <sys/msg.h> 
#include <stdlib.h>
#include <assert.h>
#include <string.h>

typedef struct massage_s
{
    long type;
    char text[100];
} message_t;

enum types {READ = 0, READ_OR_MSGFLG = 1};
int const MAX_SIZE_M_TEST = sizeof(message_t);
const int FAIL = -1;

int MassageQueueReader()
{
    message_t message = {0};
    key_t key = ftok("./massage_queue.txt", 65);
    int msgid = msgget(key, 0666 | IPC_CREAT); 
    assert(FAIL != msgid);
    
    msgrcv(msgid, &message, MAX_SIZE_M_TEST, 1, 0);
    printf("Data Received is : %s \n", message.text); 
    
    return (EXIT_SUCCESS);
}

int main()
{
    if (EXIT_FAILURE == MassageQueueReader())
    {
        return (EXIT_FAILURE);
    }

    return (EXIT_SUCCESS);
}