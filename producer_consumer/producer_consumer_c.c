#include <stdio.h> /* perror */
#include <stdlib.h> /* exit status */
#include <pthread.h> /* pthread_create */

#include "doubly_linked_list.h"

#define MAX_PRODUCERS (7)
#define MAX_CONSUMER (7)
#define UNUSED(a) ((void)(a))

dll_t *list;
pthread_mutex_t doubly_ll_mutex = PTHREAD_MUTEX_INITIALIZER;

static int JoinThreads(pthread_t *thread_producer, pthread_t *thread_consumer);
static int CreateThreads(pthread_t *thread_producer, pthread_t *thread_consumer);
static void *Push(void *param);
static void *Pop(void *param);

int main()
{
	pthread_t thread_producer[MAX_PRODUCERS], thread_consumer[MAX_CONSUMER];

	list = DLLCreate();
	if (NULL == list)
	{
		return (EXIT_FAILURE);
	}

	if (EXIT_FAILURE == CreateThreads(thread_producer, thread_consumer))
	{
		JoinThreads(thread_producer, thread_consumer);

		return (EXIT_FAILURE);
	}

	if (EXIT_SUCCESS != JoinThreads(thread_producer, thread_consumer))
	{
		return (EXIT_FAILURE);
	}

	DLLDestroy(list);
	list = NULL;

	return 0;
}

static int JoinThreads(pthread_t *thread_producer, pthread_t *thread_consumer)
{
	int i = 0;

	for (; i < MAX_PRODUCERS; ++i)
	{
		if (EXIT_SUCCESS != pthread_join (thread_producer[i], NULL))
		{
			puts("pthread_join failed:");
			DLLDestroy(list);
			list = NULL;
		
			return (EXIT_FAILURE);
		}

		if (EXIT_SUCCESS != pthread_join (thread_consumer[i], NULL))
		{
			puts("pthread_join failed:");
			DLLDestroy(list);
			list = NULL;
		
			return (EXIT_FAILURE);
		}
	}

	return (EXIT_SUCCESS);
}

static int CreateThreads(pthread_t *thread_producer, pthread_t *thread_consumer)
{
	int i = 0;

	for (; i < MAX_PRODUCERS; ++i)
	{
		if (EXIT_SUCCESS != pthread_create (&thread_producer[i], NULL,Push, &i))
		{
			perror("Create threads failed:");
			DLLDestroy(list);
			list = NULL;

			return (EXIT_FAILURE);
		}

		if (EXIT_SUCCESS != pthread_create (&thread_consumer[i], NULL, Pop, NULL))
		{
			perror("Create threads failed:");
			DLLDestroy(list);
			list = NULL;	

			return (EXIT_FAILURE);
		}
	}

	return (EXIT_SUCCESS);
}

static void *Push(void *param)
{
	size_t size_dlist = 0;

	while(1)
	{
		if (EXIT_SUCCESS != pthread_mutex_lock (&doubly_ll_mutex))
		{
			puts("mutex_lock failed! ");
			exit(EXIT_FAILURE);
		}	

		if (DLLIsSameIterator(DLLPushFront(list, &param), DLLEnd(list)))
		{
			puts ("DLLPush failed: ");

			return (NULL);
		}

		size_dlist = DLLGetSize(list);
		
		if (EXIT_SUCCESS != pthread_mutex_unlock (&doubly_ll_mutex))
		{
			puts("mutex_lock failed! ");
			exit(EXIT_FAILURE);
		}
		
		printf("push: %lu\n", size_dlist);
	}

	return (NULL);
}

static void *Pop(void *param)
{
	size_t size_dlist = 0;
	int done = 1;

	UNUSED(param);

	while(1)
	{
		done = 1;
		
		while(done)
		{
			if (EXIT_SUCCESS != pthread_mutex_lock (&doubly_ll_mutex))
			{
				puts("mutex_lock failed! ");
				exit(EXIT_FAILURE);
			}	
			
			if (!DLLIsEmpty(list))
			{		
				DLLPopFront(list);
				size_dlist = DLLGetSize(list);
				done = 0;
			}
			
			if (EXIT_SUCCESS != pthread_mutex_unlock (&doubly_ll_mutex))
			{
				puts("mutex_lock failed! ");
				exit(EXIT_FAILURE);
			}

			printf("pop: %lu\n", size_dlist);
		}
	}

	return (NULL);
}

