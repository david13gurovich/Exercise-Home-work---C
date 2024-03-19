#include <assert.h> /* assert */
#include <semaphore.h> /* sem_t */
#include <stdio.h> /* perror */
#include <stdlib.h> /* exit status */
#include <sys/types.h> /* getpid */
#include <pthread.h> /* pthread_create */

#include "fixed_size_circular_list.h"

#define MAX_PRODUCERS (3)
#define MAX_CONSUMER (3)
#define UNUSED(a) ((void)(a))
#define capacity (20)

static CList_t *c_list;
static pthread_mutex_t c_list_mutex = PTHREAD_MUTEX_INITIALIZER;
static sem_t read_sem;
static sem_t write_sem;

static void Destroy();
static void *WriteToList(void *param);
static void *ReadFromList (void *param);
static int CreateThreads(pthread_t *thread_producer, pthread_t *thread_consumer);
static int JoinThreads(pthread_t *thread_producer, pthread_t *thread_consumer);

int main()
{
	pthread_t thread_producer[MAX_PRODUCERS], thread_consumer[MAX_CONSUMER];
	
	if (EXIT_SUCCESS != sem_init (&read_sem, 0, 0))
	{
		return (EXIT_FAILURE);
	}

	c_list = CListCreate(capacity);
	if (NULL == c_list)
	{
		sem_destroy(&read_sem);
		
		return (EXIT_FAILURE);
	}
	
	if (EXIT_SUCCESS != sem_init (&write_sem, 0, capacity))
	{
		sem_destroy(&read_sem);
		CListDestroy(c_list);
		
		return (EXIT_FAILURE);
	}

	if (EXIT_FAILURE == CreateThreads(thread_producer, thread_consumer))
	{
		JoinThreads(thread_producer, thread_consumer);
		Destroy();

		return (EXIT_FAILURE);
	}

	if (EXIT_SUCCESS != JoinThreads(thread_producer, thread_consumer))
	{
		Destroy();

		return (EXIT_FAILURE);
	}

	Destroy();

	return (EXIT_SUCCESS);
}

static void Destroy()
{
	sem_destroy(&write_sem);
	sem_destroy(&read_sem);
	CListDestroy(c_list);

	return;
}

static int JoinThreads(pthread_t *thread_producer, pthread_t *thread_consumer)
{
	int i = 0;

	for (; i < MAX_PRODUCERS; ++i)
	{
		if (EXIT_SUCCESS != pthread_join (thread_producer[i], NULL))
		{
			puts("pthread_join failed:");
			Destroy();

			return (EXIT_FAILURE);
		}

		if (EXIT_SUCCESS != pthread_join (thread_consumer[i], NULL))
		{
			puts("pthread_join failed:");
			Destroy();
		
			return (EXIT_FAILURE);
		}
	}

	return (EXIT_SUCCESS);
}

static int CreateThreads(pthread_t *thread_producer, pthread_t *thread_consumer)
{
	int i = 0;

	for (i = 0; i < MAX_PRODUCERS; ++i)
	{
		if (EXIT_SUCCESS != pthread_create (&thread_producer[i], NULL,WriteToList, NULL))
		{
			perror("Create threads failed:");
			Destroy();

			return (EXIT_FAILURE);
		}

		if (EXIT_SUCCESS != pthread_create (&thread_consumer[i], NULL, ReadFromList, NULL))
		{
			perror("Create threads failed:");
			Destroy();

			return (EXIT_FAILURE);
		}
	}

	return (EXIT_SUCCESS);
}

static void *WriteToList(void *param)
{
	size_t wrote_bytes = 0;
	char copy_to_c_list = 4;	

	UNUSED(param);

	while(1)
	{
		if (EXIT_SUCCESS != sem_wait (&write_sem))
		{
				perror("sem_wait failed: ");
				exit(EXIT_FAILURE);
		}

		assert(pthread_mutex_lock (&c_list_mutex));
		wrote_bytes = CListWrite(c_list, &copy_to_c_list, sizeof(char)); 
	
		if (EXIT_SUCCESS != sem_post (&read_sem))
		{
			perror("sem_post failed: ");
			exit(EXIT_FAILURE);
		}
		assert(pthread_mutex_unlock (&c_list_mutex));

		printf("id_thread: %lu, WriteToList: %lu\n" , pthread_self(), wrote_bytes);
	}

	return (NULL);
}

static void *ReadFromList(void *param)
{
	size_t read_bytes = 0;
	char copy_from_c_list = 5;	

	UNUSED(param);
		
	while(1)
	{
		if (EXIT_SUCCESS != sem_wait (&read_sem))
		{
			perror("sem_wait failed: ");
			exit(EXIT_FAILURE);
		}
		
		assert(pthread_mutex_lock (&c_list_mutex));
		read_bytes = CListRead(c_list, &copy_from_c_list, sizeof(char));		
		if (EXIT_SUCCESS != sem_post (&write_sem))
		{
			perror("sem_post failed: ");
			exit(EXIT_FAILURE);
		}
		assert(pthread_mutex_unlock (&c_list_mutex));

		printf("id_thread: %lu, ReadFromList: %lu\n", pthread_self(), read_bytes);
	}

	return (NULL);
}

	
