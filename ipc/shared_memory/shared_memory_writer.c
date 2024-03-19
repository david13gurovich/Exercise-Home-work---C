#define _GNU_SOURCE
#include <stdio.h> /* printf */
#include <stdlib.h> /* exit_status */
#include <string.h> /* strcpy */
#include <sys/ipc.h> /* ftok */
#include <sys/ipc.h> /* shmget */
#include <sys/shm.h> /* shmget, shmat */
#include <sys/types.h> /* ftok, shmat*/
#include <unistd.h> /* getpagesize */

const int FAIL = -1;

/* int main()
{
    const void *sys_choose_addr = NULL;
    char *to_attach = NULL;
    
    key_t key = ftok("./shared_memory.txt", 65);
    
    int shmid = shmget(key, getpagesize(), IPC_CREAT|0777);
    if (FAIL == shmid)
    {
        printf("shmget failed, line: %d\n", __LINE__);
        perror("");
        return (EXIT_FAILURE);
    }

    to_attach = (char*) shmat(shmid,sys_choose_addr,0); 
    if ((void *)FAIL == to_attach)
    {
        printf("shmat failed, line: %d\n", __LINE__);
        perror("");
        return (EXIT_FAILURE);
    }
    strcpy(to_attach, "shared memory - TEXT!\n");
    
    if (FAIL == shmdt(to_attach))
    {
        printf("shmdt failed, line: %d\n", __LINE__);
        return (EXIT_FAILURE);    
    }
    
    return (EXIT_SUCCESS);
} */

typedef struct shm_s 
{
    int complete;
    char buf[100];
} shm_t;

int main()
{
    shm_t *shm_ptr = NULL;
    char *to_write = "shared memory - TEXT!";
    key_t key = ftok("./shared_memory.txt",65); 
    
    int shmid = shmget(key, sizeof(shm_t), 0666|IPC_CREAT); 
    if (FAIL == shmid)
    {
        printf("shmget failed, line: %d\n", __LINE__);
        perror("");
        return (EXIT_FAILURE);
    }

    shm_ptr = shmat(shmid,NULL ,0);
    if ((void *)FAIL == shm_ptr)
    {
        printf("shmat failed, line: %d\n", __LINE__);
        perror("");
        return (EXIT_FAILURE);
    }

    shm_ptr->complete = 0;
    strcpy(shm_ptr->buf, to_write);
    sleep(5);
    shm_ptr->complete = 1;

    if (FAIL == shmdt(shm_ptr))
    {
        printf("shmdt failed, line: %d\n", __LINE__);
        return (EXIT_FAILURE);    
    }

    if (FAIL == shmctl(shmid, IPC_RMID, 0)) 
    {
        printf("shmctl failed, line: %d\n", __LINE__);
        perror("");
        return (EXIT_FAILURE);    
    }

    return 0;
}