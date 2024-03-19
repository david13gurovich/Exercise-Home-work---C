#include <assert.h> /* assert*/
#include <semaphore.h> /* sem_t */
#include <stdio.h> /* perror */
#include <stdlib.h> /* exit status */
#include <sys/types.h> /* getpid */
#include <pthread.h> /* pthread_create */

#define MAX_PRODUCERS (1)
#define MAX_CONSUMER (2)
#define UNUSED(a) ((void)(a))

static pthread_mutex_t lock_shared_data;
static sem_t non_read_consumer;
static volatile int consumers_read = 0;
static pthread_cond_t consumers_read_cv;
static int g_shared_data = 0;

static int CreateThreads(pthread_t *thread_producer, pthread_t *thread_consumer);
static int JoinThreads(pthread_t *thread_producer, pthread_t *thread_consumer);
static void *WriteOnGdata(void *param);
static void *ReadFromGData(void *param);

int main()
{
	pthread_t thread_producer[MAX_PRODUCERS], thread_consumer[MAX_CONSUMER];
	if (EXIT_FAILURE == pthread_cond_init (&consumers_read_cv, NULL))
	{
		return (EXIT_FAILURE);
	}

	if (EXIT_SUCCESS != sem_init (&non_read_consumer, 0, 0))
	{
		pthread_cond_destroy(&consumers_read_cv);
		
		return (EXIT_FAILURE);
	}

	if (EXIT_SUCCESS != CreateThreads(thread_producer, thread_consumer))
	{
		JoinThreads(thread_producer, thread_consumer);
		sem_destroy(&non_read_consumer);
		pthread_cond_destroy(&consumers_read_cv);

		return (EXIT_FAILURE);
	}

	if (EXIT_SUCCESS != JoinThreads(thread_producer, thread_consumer))
	{
		sem_destroy(&non_read_consumer);
		pthread_cond_destroy(&consumers_read_cv);

		return (EXIT_FAILURE);
	}

	sem_destroy(&non_read_consumer);
	pthread_cond_destroy(&consumers_read_cv);

	return (EXIT_SUCCESS);
}

static int CreateThreads(pthread_t *thread_producer, pthread_t *thread_consumer)
{
	int i = 0;

	for (i = 0; i < MAX_PRODUCERS; ++i)
	{
		if (EXIT_SUCCESS != pthread_create (&thread_producer[i], NULL,WriteOnGdata, NULL))
		{
			perror("Create threads failed:");
			sem_destroy(&non_read_consumer);;

			return (EXIT_FAILURE);
		}

		if (EXIT_SUCCESS != pthread_create (&thread_consumer[i], NULL, ReadFromGData, NULL))
		{
			perror("Create threads failed:");
			sem_destroy(&non_read_consumer);;

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
			sem_destroy(&non_read_consumer);;
		
			return (EXIT_FAILURE);
		}

		if (EXIT_SUCCESS != pthread_join (thread_consumer[i], NULL))
		{
			puts("pthread_join failed:");
			sem_destroy(&non_read_consumer);;
		
			return (EXIT_FAILURE);
		}
	}

	return (EXIT_SUCCESS);
}

static void *WriteOnGdata(void *param)
{
	int consumers = 0;
	int assert_stat = 0;

	UNUSED(param);
	
	while(1)
	{
		assert_stat = pthread_mutex_lock (&lock_shared_data);
		assert(!assert_stat);

		++g_shared_data;

		for (consumers = 0; consumers < MAX_CONSUMER; ++consumers)
		{
			if (EXIT_SUCCESS != sem_post (&non_read_consumer))
			{
				perror("sem_post failed: ");
				exit(EXIT_FAILURE);
			}
		}
		
		while (consumers_read != MAX_CONSUMER)
		{
			if (pthread_cond_wait (&consumers_read_cv, &lock_shared_data))
			{
				perror("pthread_cond_wait failed: ");
				exit(EXIT_FAILURE);
			}
		}
		
		consumers_read = 0;
		assert_stat = pthread_mutex_unlock (&lock_shared_data);
		assert(!assert_stat);
	}

	return (NULL); 
}

static void *ReadFromGData(void *param)
{
	int assert_stat = 0;
	
	UNUSED(param);

	while(1)
	{
		if (EXIT_SUCCESS != sem_wait (&non_read_consumer))
		{
			perror("sem_wait failed: ");
			exit(EXIT_FAILURE);
		}

		assert_stat = pthread_mutex_lock (&lock_shared_data);
		assert(!assert_stat);
		printf("%d\n", g_shared_data);
		++consumers_read;
		assert_stat = pthread_cond_signal (&consumers_read_cv);
		assert(!assert_stat); 
		assert_stat = pthread_mutex_unlock (&lock_shared_data);
		assert(!assert_stat); 	
}

	return (NULL); 	
}