#include <assert.h> /* assert */
#include <stdio.h> /* perror */
#include <stdlib.h> /* exit status */
#include <pthread.h> /* pthread_create */

#define UNUSED(a) ((void)(a))

static int g_data_to_shared = 0;
static int g_data_lock = 0;

static void *WriteOnGData(void *param);
static void *ReadGData(void *param);

enum {CAN_WRITE = 0, CAN_READ = 1};

int main()
{
	pthread_t thread_producer, thread_consumer;
	pthread_mutex_t data_mutex = PTHREAD_MUTEX_INITIALIZER; 
	
	if (EXIT_SUCCESS != pthread_create (&thread_producer, NULL, &WriteOnGData, &data_mutex))
	{
		perror("Create threads failed:");

		return (EXIT_FAILURE);
	}

	if (EXIT_SUCCESS != pthread_create (&thread_consumer, NULL, &ReadGData, &data_mutex))
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
	while (1)	
	{
		if (EXIT_SUCCESS != pthread_mutex_lock ((pthread_mutex_t *)param))
		{
			puts("mutex_lock failed! ");
			exit(EXIT_FAILURE);
		}

		if (CAN_WRITE == g_data_lock)
		{
			printf("write in data, g_var: %d\n", g_data_to_shared);
			g_data_lock = 1;
			++g_data_to_shared;
		}

		if (EXIT_SUCCESS != pthread_mutex_unlock ((pthread_mutex_t *)param))
		{
			puts("mutex_lock failed! ");
			exit(EXIT_FAILURE);
		}
	}

	return (NULL);
}

static void *ReadGData(void *param)
{
	while (1)
	{
		if (EXIT_SUCCESS != pthread_mutex_lock ((pthread_mutex_t *)param))
		{
			puts("mutex_lock failed! ");
			exit(EXIT_FAILURE);
		}

		if (CAN_READ == g_data_lock)	
		{
			printf("read data, g_var: %d\n", g_data_to_shared);
			g_data_lock = CAN_WRITE;
		}

		if (EXIT_SUCCESS != pthread_mutex_unlock ((pthread_mutex_t *)param))
		{
			puts("mutex_lock failed! ");
			exit(EXIT_FAILURE);
		}
	}

	return (NULL);
}	