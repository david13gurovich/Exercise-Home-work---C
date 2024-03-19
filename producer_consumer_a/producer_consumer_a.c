#include <stdio.h> /* perror */
#include <stdlib.h> /* exit status */
#include <pthread.h> /* pthread_create */

#define UNUSED(a) ((void)(a))

static int g_data_to_shared = 0;
static int g_data_lock = 0;

static void *WriteOnGData(void *param);
static void *ReadGData(void *param);

const int SUCCESS = 0;
const int RUN_LOOP = 10;

int main()
{
	pthread_t thread_producer, thread_consumer;
		
	if (SUCCESS != pthread_create (&thread_producer, NULL, &WriteOnGData, NULL))
	{
		perror("Create threads failed:");

		return (EXIT_FAILURE);
	}

	if (SUCCESS != pthread_create (&thread_consumer, NULL, &ReadGData, NULL))
	{
		perror("Create threads failed:");

		return (EXIT_FAILURE);
	}

	if (SUCCESS != 	pthread_join (thread_producer, NULL))
	{
		puts("pthread_join failed:");
		
		return (EXIT_FAILURE);
	}

	if (SUCCESS != 	pthread_join (thread_consumer, NULL))
	{
		puts("pthread_join failed:");
		
		return (EXIT_FAILURE);
	}

	return (EXIT_SUCCESS);
}

static void *WriteOnGData(void *param)
{
	UNUSED(param);

	while (g_data_to_shared < 10)	
	{
		if (0 == g_data_lock)
		{
			printf("write in data, g_var: %d\n", g_data_to_shared);
			g_data_lock = 1;
			++g_data_to_shared;
		}
	}

	return (NULL);
}

static void *ReadGData(void *param)
{
	UNUSED(param);
	
	while (g_data_to_shared < 10)	
	{
		if (1 == g_data_lock)
		{
			printf("read data, g_var: %d\n", g_data_to_shared);
			g_data_lock = 0;
		}
	}

	return (NULL);
}	