#include <stdio.h> 
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <stdlib.h>
#include <assert.h> 

const int FAIL = -1;

int namePipes()
{
    int fd = 0;
    int fd2 = 0;
    char *path_file = "./my_file";
    char *path_file2 = "./my_file2";
    char to_write[] = "Pong";
    char to_read[100] = {0};

    fd = open(path_file, O_RDONLY);
    if (FAIL == fd)
    {
        printf("open failed, line: %d\n", __LINE__);
        perror("");
        return (EXIT_FAILURE);
    }
    fd2 = open(path_file2 ,O_WRONLY);
    if (FAIL == fd2)
    {
        printf("open failed, line: %d\n", __LINE__);
        perror("");
        return (EXIT_FAILURE);
    }
    
    while(1)
    {
        assert(FAIL != write(fd2, to_write, strlen(to_write) + 1));
        if (FAIL == read(fd, to_read, sizeof(to_read)))
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
/*    PipeAndFork();   */

    namePipes();
    
    return 0;
}