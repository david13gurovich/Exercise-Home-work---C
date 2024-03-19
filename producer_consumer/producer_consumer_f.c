#include <assert.h> /* assert*/
#include <semaphore.h> /* sem_t */
#include <stdio.h> /* perror */
#include <stdlib.h> /* exit status */
#include <sys/types.h> /* getpid */
#include <pthread.h> /* pthread_create */

#include "fixed_size_circular_list.h"

#define MAX_PRODUCERS (1)
#define MAX_CONSUMER (1)
#define UNUSED(a) ((void)(a))
#define capacity (100)

static CList_t *c_list;
static pthread_mutex_t write_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t read_mutex = PTHREAD_MUTEX_INITIALIZER;
static sem_t free_space_to_write;
static sem_t avliable_to_read;

static void Destroy();
static void *WriteToList(void *param);
static void *ReadFromList (void *param);
static int CreateThreads(pthread_t *thread_producer, pthread_t *thread_consumer);
static int JoinThreads(pthread_t *thread_producer, pthread_t *thread_consumer);

int main()
{
	pthread_t thread_producer[MAX_PRODUCERS], thread_consumer[MAX_CONSUMER];
	
	if (EXIT_SUCCESS != sem_init (&avliable_to_read, 0, 0))
	{
		return (EXIT_FAILURE);
	}

	c_list = CListCreate(capacity);
	if (NULL == c_list)
	{
		sem_destroy(&avliable_to_read);
		
		return (EXIT_FAILURE);
	}
	
	if (EXIT_SUCCESS != sem_init (&free_space_to_write, 0, capacity))
	{
		sem_destroy(&free_space_to_write);
		CListDestroy(c_list);
		
		return (EXIT_FAILURE);
	}

	if (EXIT_SUCCESS != CreateThreads(thread_producer, thread_consumer))
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
	sem_destroy(&free_space_to_write);
	sem_destroy(&avliable_to_read);
	CListDestroy(c_list);

	return;
}

static int CreateThreads(pthread_t *thread_producer, pthread_t *thread_consumer)
{
	int i = 0;

	for (i = 0; i < MAX_PRODUCERS; ++i)
	{
		if (EXIT_SUCCESS != pthread_create (&thread_producer[i], NULL,WriteToList, NULL))
		{
			perror("Create threads failed:");
			sem_destroy(&free_space_to_write);
			sem_destroy(&avliable_to_read);
			CListDestroy(c_list);

			return (EXIT_FAILURE);
		}

		if (EXIT_SUCCESS != pthread_create (&thread_consumer[i], NULL, ReadFromList, NULL))
		{
			perror("Create threads failed:");
			sem_destroy(&free_space_to_write);
			sem_destroy(&avliable_to_read);
			CListDestroy(c_list);

			return (EXIT_FAILURE);
		}
	}

	return (EXIT_SUCCESS);
}

static int JoinThreads(pthread_t *thread_producer, pthread_t *thread_consumer)
{
	int i = 0;

	for (; i < MAX_PRODUCERS; ++i)
	{
		if (EXIT_SUCCESS != pthread_join (thread_producer[i], NULL))
		{
			puts("pthread_join failed:");
			sem_destroy(&free_space_to_write);
			sem_destroy(&avliable_to_read);
			CListDestroy(c_list);
		
			return (EXIT_FAILURE);
		}

		if (EXIT_SUCCESS != pthread_join (thread_consumer[i], NULL))
		{
			puts("pthread_join failed:");
			sem_destroy(&free_space_to_write);
			sem_destroy(&avliable_to_read);
			CListDestroy(c_list);
		
			return (EXIT_FAILURE);
		}
	}

	return (EXIT_SUCCESS);
}

static void *WriteToList(void *param)
{
	size_t wrote_bytes = 0;
	char copy_to_c_list = 1;	

	UNUSED(param);

	while(1)
	{
		if (EXIT_SUCCESS != sem_wait (&free_space_to_write))
		{
			perror("sem_wait failed: ");
			exit(EXIT_FAILURE);
		}

		assert (pthread_mutex_lock (&write_mutex));
		wrote_bytes = CListWrite(c_list, &copy_to_c_list, sizeof(char));
		assert (pthread_mutex_unlock (&write_mutex));
		
		if (EXIT_SUCCESS != sem_post (&avliable_to_read))
		{
			perror("sem_post failed: ");
			exit(EXIT_FAILURE);
		}
		
		printf("id_thread: %lu, WriteToList: %lu\n" , pthread_self(), wrote_bytes);
	}

	return (NULL);
}

static void *ReadFromList(void *param)
{
	size_t read_bytes = 0;
	char copy_from_c_list = 1;	

	UNUSED(param);
		
	while(1)
	{
		if (EXIT_SUCCESS != sem_wait (&avliable_to_read))
		{
			perror("sem_wait failed: ");
			exit(EXIT_FAILURE);
		}
		
		assert(pthread_mutex_lock (&read_mutex));
		read_bytes = CListRead(c_list, &copy_from_c_list, sizeof(char)); 
		assert(pthread_mutex_unlock (&read_mutex));

		if (EXIT_SUCCESS != sem_post (&free_space_to_write))
		{
			perror("sem_post failed: ");
			exit(EXIT_FAILURE);
		}
		
		printf("id_thread: %lu, ReadFromList: %lu\n", pthread_self(), read_bytes);
	}

	return (NULL);
}

	
