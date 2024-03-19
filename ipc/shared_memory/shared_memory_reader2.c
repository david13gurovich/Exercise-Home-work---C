#define _GNU_SOURCE
#include <assert.h>
#include <stdio.h> /* printf */
#include <stdlib.h> /* exit_status */
#include <string.h>  /* strcpy */
#include <sys/ipc.h> /* ftok */
#include <sys/ipc.h> /* shmget */
#include <sys/shm.h> /* shmget, shmat */
#include <sys/types.h> /* ftok, shmat*/
#include <unistd.h> /* getpagesize */

const int FAIL = -1;
/*
int main()
{
    int shmid = 0;
    char *str = NULL;

    key_t key = ftok("./shared_memory.txt",65); 
  
    shmid = shmget(key,getpagesize(),0666|IPC_CREAT); 
    if (FAIL == shmid)
    {
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
    
    assert(FAIL != shmctl(shmid,IPC_RMID,NULL)); 

    return 0;
}
 */
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

    shmctl(shmid,IPC_RMID,NULL); 

    return (EXIT_SUCCESS);
}
 