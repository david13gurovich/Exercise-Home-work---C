#include <pthread.h> /*pthread_create, pthread_join, pthread_t*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

extern void *etext;
extern void *edata;
extern void *end;

typedef struct params
{
	int id;
	char *proceses_or_thread;
} params_t;

typedef void *(* func)(void *params);

void *PrintAdress(void *params);

int main()
{
	char *params = "THREAD!";
	char *params_process = "PROCESS";
	pthread_t thread = 0;

	PrintAdress(params_process);	
	
	pthread_create(&thread, NULL, PrintAdress, params);
	pthread_join(thread, NULL);	

	return 0;
}

void *PrintAdress(void *params)
{
	char command[100];
	char *adress_in_heap = malloc(10);
	if (NULL == adress_in_heap)
	{
		return (NULL);
	}
	
	printf("\n%s\n\n", (char *)params);
	
	sprintf(command, "cat /proc/%d/maps | grep stack | awk '{print $1}'",  getpid());
	printf("stack addresses\n"); 
	system(command);

	sprintf(command, "cat /proc/%d/maps | grep heap | awk '{print $1}'",  getpid());
	printf("heap addresses\n"); 
	system(command);

	printf("text addresses %p - %p \n", &main, &etext-1);
	printf("data addresses %p - %p \n", &etext, &edata-1);
	printf("bss addresses %p - %p \n", &edata, &end-1);
	
	printf("malloc adress: %p\n", adress_in_heap);

	return (NULL);
}