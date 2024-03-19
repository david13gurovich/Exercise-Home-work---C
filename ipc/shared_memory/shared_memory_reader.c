#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h> /* ftok, shmat*/
#include <sys/ipc.h> /* ftok */
#include <unistd.h> /* getpagesize */
#include <sys/ipc.h> /* shmget */
#include <sys/shm.h> /* shmget, shmat */
#include <string.h>

const int FAIL = -1;

/* int main()
{
    char *str = NULL;
    key_t key = ftok("./shared_memory.txt",65); 
    int shmid = shmget(key,getpagesize(),0666|IPC_CREAT); 
    if (FAIL == shmid)
    {
        printf("shmget failed, line: %d\n", __LINE__);
        perror("");
        return (EXIT_FAILURE);
    }
    str = (char*) shmat(shmid,(void*)0,0);
     if ((void *)FAIL == str)
    {
        printf("shmat failed, line: %d\n", __LINE__);
        perror("");
        return (EXIT_FAILURE);
    } 
  
    printf("Data read from memory: %s\n",str); 
    if (FAIL == shmdt(str))
    {
        printf("shmdt failed, line: %d\n", __LINE__);
        return (EXIT_FAILURE);    
    }
    
    return EXIT_SUSSCSS;
}  */

typedef struct shm_s 
{
    int complete;
    char buf[100];
} shm_t;

int main()
{
    shm_t *shm_ptr;
    key_t key = ftok("./shared_memory.txt",65); 
    int shmid = shmget(key,sizeof(shm_t),0666|IPC_CREAT); 
    if (FAIL == shmid)
    {
        printf("shmget failed, line: %d\n", __LINE__);
        perror("");
        return (EXIT_FAILURE);
    }

    shm_ptr = shmat(shmid, NULL, 0);
    if ((void *)FAIL == shm_ptr) 
    {
      perror("Shared memory attach");
      return (EXIT_FAILURE);
    }
    
    while (shm_ptr->complete != 1);

    printf("Data read from memory: %s\n",shm_ptr->buf); 
    if (FAIL == shmdt(shm_ptr))
    {
        printf("shmdt failed, line: %d\n", __LINE__);
        return (EXIT_FAILURE);    
    }

    return (EXIT_SUCCESS);
}
 