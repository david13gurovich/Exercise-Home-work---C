#define _POSIX_C_SOURCE 199309L
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h> /* memset */z

#define UNUSED(A) ((void)(A)) 

int flag = 0; 
unsigned long other_pid; 
static int cnt_process2  = 0;

void sig_hand(int sig, siginfo_t *siginfo, void *context)
{
	UNUSED(sig);
	UNUSED(context);
	other_pid = (unsigned long)siginfo->si_pid;
	flag = 1;
}

void ProcessOne(pid_t pid)
{
	assert(pid > 0);
	
	kill(pid, SIGUSR1);
	while(0 == flag)
	{};
	
	++cnt_process2;
	printf("Pong!, %d\n", cnt_process2);
	flag = 0;
	
}

void ProcessTwo()
{
	
	while(0 == flag)
    {};
    
    ++cnt_process2;
	printf("Ping!, %d\n", cnt_process2);
    flag = 0;

    kill(other_pid, SIGUSR1);
}

int main(int argc, char const *argv[])
{
	struct sigaction sa;
    
    memset (&sa, 0, sizeof (sa)); /* init */

    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = sig_hand;
    printf("insert %d in the other terminal \n", getpid());

    if (-1 == sigaction(SIGUSR1, &sa, NULL))
    {
    	puts("sigaction failed\n");

    	return (EXIT_FAILURE);
    }    

	while(1)
	{
	    if (argc == 2) 
	    {
	    	ProcessOne(atoi(argv[1]));
	    }
	    else 
	    {
	        ProcessTwo();
	    }
	}
	
	return (EXIT_SUCCESS);
}
