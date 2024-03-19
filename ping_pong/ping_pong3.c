#define _POSIX_C_SOURCE 199309L
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h> /* memset */

#define UNUSED(A) ((void)(A)) 

int flag = 0; 

void sig_hand(int sig, siginfo_t *siginfo, void *context)
{
	UNUSED(sig);
	UNUSED(context);
	UNUSED(siginfo);
	flag = 1;
}

void ProcessOne(pid_t pid)
{
	assert(pid > 0);
	
	kill(pid, SIGUSR1);
	
	while(0 == flag)
	{};
	
	printf("Pong!\n");
	flag = 0;
}

void ProcessTwo(pid_t pid)
{
	while(0 == flag)
    {};
    
	printf("Ping!\n");
    flag = 0;
    kill(pid, SIGUSR1);

}

int main(int argc, char const *argv[])
{
	struct sigaction sa;
	char *list[100]= {0};
    pid_t pid; 
    pid_t pid_original_process; 
    char str_pid[100];

    list[0] = "./a.out";    
    memset (&sa, 0, sizeof (sa)); 

    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = sig_hand;
    
    if (-1 == sigaction(SIGUSR1, &sa, NULL))
    {
    	puts("sigaction failed\n");

    	return (EXIT_FAILURE);
    }  

	pid = 0;
    if (2 == argc) 
    {
    	while(1)
    	{
    		ProcessOne(atoi(argv[1]));
    	}
    }
	pid = fork();
	if (pid > 0)
	{
		while(1)
		{
			ProcessTwo(pid);
		}
	}

	if (pid == 0)
	{ 	
		pid_original_process = getppid();
		sprintf(str_pid, "%lu", (unsigned long) pid_original_process);  
		list[1] = str_pid; 
		if (execvp(list[0], list) < 0)
		{
			printf("\nERROR: execute failed!\n");
			exit(0);	
		} 
	}
	
	return (EXIT_SUCCESS);
}

