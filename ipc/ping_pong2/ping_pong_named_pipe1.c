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

const int FAIL = -1;

int namePipes()
{
    int fd = 0;
    int fd2 = 0;
    char *path_file = "./my_file";
    char *path_file2 = "./my_file2";
    char to_write[] = "Ping";
    char to_read[100] = {0};

    if (FAIL == mkfifo(path_file, 0666))
    {
        perror("");
        return (EXIT_FAILURE);
    }
    
    if(FAIL == mkfifo(path_file2, 0666))
    {
        perror("");
        return (EXIT_FAILURE);
    }

    fd = open(path_file, O_WRONLY);
    if (FAIL == fd)
    {
        printf("open failed, line: %d\n", __LINE__);
        perror("");
        return (EXIT_FAILURE);
    }
    
    fd2 = open(path_file2, O_RDONLY);
    if (FAIL == fd2)
    {
        printf("open failed, line: %d\n", __LINE__);
        perror("");
        return (EXIT_FAILURE);
    }

    while(1)
    {
        assert(FAIL != write(fd, to_write, strlen(to_write) + 1));

        if (FAIL == read(fd2, to_read, sizeof(to_read)))
        {
            printf("read failed, line: %d\n", __LINE__);
            return (EXIT_FAILURE);   
        }

        printf("%s\n", to_read);
    }
    
    return (EXIT_SUCCESS);
}

int main()
{ 
    namePipes();
    
    return 0;
}