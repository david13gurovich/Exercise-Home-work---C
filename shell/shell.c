#include <ctype.h> /* isspace */
#include <errno.h>  /* perror */
#include <stdio.h> /* printf, gets */
#include <stdlib.h> /* exit (0) */ 
#include <string.h> /* strcmp */
#include <sys/types.h> /* uid_t, pid_t */
#include <sys/wait.h> /* wait */
#include <sys/utsname.h> /* host name */
#include <pwd.h> /* getpwuid */
#include <unistd.h>  /* fork, execvp */

#include "shell.h"

static void parse(char *input, char **argv);
static void execute(char **argv);
static void Magenta();
static void Cyan();
static void Red();
static void Defult();

#define MEX_INPUT_SIZE (1024)
#define MAX_ARGV_SIZE (64)

void ExecuteShell()
{
	char input[MEX_INPUT_SIZE];
	char *argv[MAX_ARGV_SIZE];
	struct utsname host_name;
	struct passwd *usr_name;
	uid_t uid;
	
	uid = geteuid();
	usr_name = getpwuid(uid);
	if (NULL == usr_name)
	{
		perror("getpwuid faild: ");
		return;
	}

	if (-1 == uname(&host_name))
	{
		perror("uname faild: ");
		return;
	}

   	while (1)
   	{
		Cyan();		
		printf("%s$%s:", usr_name->pw_name, host_name.nodename);

	   	Magenta();
	   	if (-1 == fscanf(stdin, "%[^\n]%*c", input))
	   	{
	   		return;
	   	}
	   	
	   	Defult();
	   	printf("\n");
	   	parse(input, argv);

	   	if (!strcmp(argv[0], "exit"))
	   	{
	   		exit(0);            
	   	}

	   	execute(argv);
   	}

   	return;
}

static void parse(char *command, char **argv)
{
	while ('\0' != *command) 
	{      
		while (isspace(*command))
		{
			*command++ = '\0';    
		}

		*argv++ = command;         
		while ('\0' != *command && !isspace(*command)) 
		{
			command++;            
		}
	}
	*argv = '\0';

	return;
}

static void execute(char **argv)
{
	int status;
	pid_t  pid = fork();

	if (0 == pid)
	{
		if (execvp (*argv, argv) < 0)
		{
			Red();
			printf("\nERROR: execute failed!\n");
			exit(0);	
		} 
	}
	else if (pid > 0)
	{
		while (wait(&status) != pid);
	}
	else
	{
		Red();
		printf("\nERROR: fork failed!\n");
		exit(0);
	}

	return;
}

static void Magenta() 
{
  printf("\033[1;35m");
}

static void Cyan()
{
	printf("\033[1;36m");
}

static void Red()
{
  printf("\033[1;31m");
}

static void Defult()
{
	printf("\033[0m"); 
}