#include <stdio.h> /* perror */
#include <stdlib.h> /* exit status */
#include <pthread.h> /* pthread_create */

#define UNUSED(a) ((void)(a))

static volatile int g_data_to_shared = 0;
static volatile int g_data_lock = 0;

static void *WriteOnGData(void *param);
static void *ReadGData(void *param);

enum {CAN_WRITE = 0, CAN_READ = 1};

int main()
{
	pthread_t thread_producer = 0, thread_consumer = 0;
		
	if (EXIT_SUCCESS != pthread_create (&thread_producer, NULL, &WriteOnGData, NULL))
	{
		perror("Create threads failed:");

		return (EXIT_FAILURE);
	}

	if (EXIT_SUCCESS != pthread_create (&thread_consumer, NULL, &ReadGData, NULL))
	{
		perror("Create threads failed:");
		pthread_join (thread_producer, NULL);

		return (EXIT_FAILURE);
	}

	if (EXIT_SUCCESS != pthread_join (thread_producer, NULL))
	{
		puts("pthread_join failed:");
		
		return (EXIT_FAILURE);
	}

	if (EXIT_SUCCESS != pthread_join (thread_consumer, NULL))
	{
		puts("pthread_join failed:");
		
		return (EXIT_FAILURE);
	}

	return (EXIT_SUCCESS);
}

static void *WriteOnGData(void *param)
{
	UNUSED(param);

	while (1)	
	{
		if (CAN_WRITE == g_data_lock)
		{
			printf("write in data, g_var: %d\n", g_data_to_shared);
			g_data_lock = CAN_READ;
			++g_data_to_shared;
		}
	}

	return (NULL);
}

static void *ReadGData(void *param)
{
	UNUSED(param);
	
	while (1)
	{
		if (CAN_READ == g_data_lock)	
		{
			printf("read data, g_var: %d\n", g_data_to_shared);
			g_data_lock = CAN_WRITE;
		}
	}

	return (NULL);
}	