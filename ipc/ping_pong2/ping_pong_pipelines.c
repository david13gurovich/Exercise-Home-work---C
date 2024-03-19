#define _GNU_SOURCE             
#include <fcntl.h>              
#include <unistd.h> /* pipe, fork */
#include <sys/types.h> /* fork, mkfifo */
#include <stdlib.h> /* exit_status */
#include <stdio.h> /* stderr */
#include <sys/wait.h> /* wait */
#include <string.h> /* strlen */
#include <assert.h> 
#include <sys/stat.h> /* mkfifo */

enum { READ = 0, WRITE = 1};
const int FAIL = -1;

int Toclose(int *fd_close_read, int *fd_close_write)
{
    if (FAIL == close(fd_close_read[READ]))
    {
        printf("close failed, line: %d\n", __LINE__);
        perror("");
        return (EXIT_FAILURE);
    }

    if (FAIL == close(fd_close_write[WRITE]))
    {
        printf("close failed, line: %d\n", __LINE__);
        perror("");
        return (EXIT_FAILURE);
    }

    return (EXIT_SUCCESS);
}

int PipeAndFork()
{
    int fd1[2], fd2[2];
    char *ping = "ping", *pong = "pong"; 
    const size_t len_massage = strlen(ping); 
    char parent_buf[100] = {0}, child_buf[100] = {0};
    pid_t pid = 0;

    if (FAIL == pipe(fd1) || FAIL == pipe(fd2))
    {
        return (EXIT_FAILURE);
    }
    
    pid = fork();

    if(pid > 0)
    {
        if(EXIT_FAILURE == Toclose(fd1, fd2))
        {
            return (EXIT_FAILURE);   
        }

        while(1)
        {
            assert (FAIL != write(fd1[WRITE], ping, len_massage + 1));

            if(FAIL == read(fd2[READ], parent_buf, len_massage + 1))
            {
                printf("read failed, line: %d\n", __LINE__);
                return (EXIT_FAILURE);   
            }
            printf("%s\n", parent_buf);
        }
    }
    else if(0 == pid)
    {
        if(EXIT_FAILURE == Toclose(fd2, fd1))
        {
            return (EXIT_FAILURE);   
        }
        
        while(1)
        {
            if (FAIL == read(fd1[READ], child_buf, len_massage + 1))
            {
                printf("read failed, line: %d\n", __LINE__);
                return (EXIT_FAILURE);       
            }

            assert(FAIL != write(fd2[WRITE], pong, len_massage + 1));
            printf("%s\n", child_buf);
        }
    }
    else
    {
        fprintf(stderr, "fork Failed" );
        return (EXIT_FAILURE); 
    }

    return (EXIT_SUCCESS);
}

int main()
{ 
    PipeAndFork();  
    
    return 0;
}