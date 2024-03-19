#include <stdio.h> /* printf, gets */
#include <stdlib.h> /* exit_status */ 
#include <sys/types.h> /* uid_t, pid_t */
#include <sys/wait.h> /* wait */
#include <unistd.h>  /* fork, execvp */

int main(int argc, char const *argv[])
{
	int status;
	pid_t pid = 0;

	if (argc < 2)
	{
		puts("need to add an argumant\n");

		return (EXIT_FAILURE);
	}

	while(1)
	{
		pid = fork();
		
		if (0 == pid)
		{
			if (execvp (argv[1], (char **)&argv[1]) < 0)
			{
				printf("\nERROR: execvp failed!\n");
				
				return (EXIT_FAILURE);	
			} 
		}
		else if (pid > 0)
		{
			wait(&status);
		}
		else
		{
			printf("\nERROR: fork failed!\n");
		}

	}

	return (EXIT_SUCCESS);
}