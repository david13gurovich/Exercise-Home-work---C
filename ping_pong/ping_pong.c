#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h> /* memset */

#define errExit(MSG) do { perror(MSG); return(EXIT_FAILURE); } while (0)
#define UNUSED(A) ((void)(A)) 

int flag = 0; 

void sig_hand(int sig) 
{
	UNUSED(sig);

	flag = 1;
}

int main()
{
	pid_t pid;
    struct sigaction sa;
    memset (&sa, 0, sizeof (sa)); /* init */

    sa.sa_flags = 0;
    sa.sa_handler = sig_hand;

    if (-1 == sigaction(SIGUSR1, &sa, NULL))
    {
    	errExit("sigaction");
    }    

    pid = fork();
	while(1)
	{
	    if (0 == pid) /* child */
	    {
	        kill(getppid(), SIGUSR1);
	        while(0 == flag)
	        {};

	        printf("Pong!\n");
	        flag = 0;
	    }
	    else if (pid > 0) /* parent */
	    {
	        while(0 == flag)
	        {};
	        printf("Ping!\n");
	        flag = 0;
	    
	        kill(pid, SIGUSR1);
	    }
	    else
	    {
	    	puts("fork failed");

	    	return (EXIT_FAILURE);
	    }
	}

    return (EXIT_SUCCESS);
}

/*int main(void) {
  struct sigaction sa;
  memset(&sa, 0, sizeof(sa));
  sa.sa_flags = SA_SIGINFO;
  sa.sa_sigaction = sigusr1;
  if (sigaction(SIGUSR1, &sa, 0) == -1) {
    fprintf(stderr, "%s: %s\n", "sigaction", strerror(errno));
  }
  printf("Pid %lu waiting for SIGUSR1\n", (unsigned long)getpid());
  for (;;) {
    sleep(10);
  }
  return 0;
}*/