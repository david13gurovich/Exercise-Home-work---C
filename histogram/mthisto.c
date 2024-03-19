#include <assert.h>
#include <ctype.h> /*tolower*/
#include <fcntl.h> /*open*/
#include <pthread.h> /*pthread_create, pthread_join, pthread_t*/
#include <semaphore.h> /*sem_t sem_wait, sem_post*/
#include <stdio.h>
#include <stdlib.h> /*malloc, size_t, EXIT_FAILURE, EXIT_SUCCESS*/
#include <unistd.h> /*close, sysconf*/

#include <sys/mman.h> /*mmap*/
#include <sys/stat.h> /*stat*/

#include "cbuffer.h"

#define NUM_OF_LETTERS (26)

typedef struct producer_s
{
	int num_of_segments;
	int *personal_histo;
} producer_t;

typedef void *(*func_ptr)(void *); 

static void Alloction(int num_of_producers, int num_of_segments, 
		producer_t **producers_param, pthread_t **producers_id);
static void *CalcSumOfHisto(void *param);
static void *CountCharInSegment(void *param);
static void CreateThreads(int num_of_segmants, int num_of_threads, 
				pthread_t *thread_id, void *param, func_ptr func);
static void DestroyAll(int num_of_producers, int num_of_segments,
					producer_t *producers_param, pthread_t *producers_id);
static void FillLocalHistogram(unsigned char *segment_ptr, int *histo_ptr);
static void InitProducerParam(producer_t *producers_param, int num_of_producers,
							 	int num_of_segments, pthread_t *producers_id);
static int JoinThreads(int num_of_threads, pthread_t *thread_id);
static int MapFileSegments(int num_of_segments, char *filename);
static void PrintTotalHistogream();
static void InitParameters(char *filename, int *size_of_file, 
										int *num_of_segments);

static cbuffer_t *g_bucket_histo = NULL;
static off_t g_size_of_segment = 0;
static int g_i_segment_arr = -1;
static int g_total_histo[NUM_OF_LETTERS];
static pthread_mutex_t g_lock_segments_arr = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t g_lock_buffer = PTHREAD_MUTEX_INITIALIZER;
static sem_t g_histo_to_read;
static unsigned char **file_segments_arr = NULL;

const int failed_status = -1;
const size_t size_of_pointer = sizeof(void *);

int main(int argc, char const *argv[])
{
	const int num_of_consumers = 1;
	char *filename = NULL;
	int num_of_producers = 0; 
	int num_of_segments = 0;
	int size_of_file = 0;
	producer_t *producers_param = NULL;
	pthread_t *producers_id = NULL, consumer_id = 0;

	if(argc < 3)
	{
		puts("add num of producers and file path");
		return (EXIT_FAILURE);
	}

	num_of_producers = atoi(argv[1]);
	filename = (char *)argv[2];

	Alloction(num_of_producers, num_of_segments, &producers_param, &producers_id);
	InitParameters(filename, &size_of_file, &num_of_segments);

	if (EXIT_FAILURE == MapFileSegments(num_of_segments, filename))
	{
		DestroyAll(num_of_producers, num_of_segments,
						producers_param, producers_id);
		return (EXIT_FAILURE);
	}

	InitProducerParam(producers_param, num_of_producers, 
						num_of_segments, producers_id);

	CreateThreads(num_of_segments, num_of_producers, producers_id,
			producers_param, CountCharInSegment);
	CreateThreads(num_of_segments, num_of_consumers, &consumer_id, 
								&num_of_producers, CalcSumOfHisto);
	
	if (EXIT_FAILURE == JoinThreads(num_of_producers, producers_id) || 
		EXIT_FAILURE == JoinThreads(num_of_consumers, &consumer_id))
	{
		DestroyAll(num_of_producers, num_of_segments ,
						producers_param, producers_id);
		return (EXIT_FAILURE);
	}

	DestroyAll(num_of_producers, num_of_segments, producers_param, producers_id);
	PrintTotalHistogream();

	return (EXIT_SUCCESS);
}

static void InitParameters(char *filename, int *size_of_file, 
										int *num_of_segments)
{
	int file_status = 0;
	struct stat st;

	file_status = stat(filename, &st);
	if (failed_status == file_status)
	{
		perror("file_status failed ");
		exit (EXIT_FAILURE);
	}
	*size_of_file = st.st_size;

	g_size_of_segment = sysconf(_SC_PAGE_SIZE);
	assert(-1 != g_size_of_segment);
	
	*num_of_segments = *size_of_file / g_size_of_segment;

	if (failed_status == sem_init (&g_histo_to_read, 0, 0))
	{
		perror("sem_init failed: ");
		exit(EXIT_FAILURE);
	} 
}

static void Alloction(int num_of_producers, int num_of_segments, 
					producer_t **producers_param, pthread_t **producers_id)
{
	const int remainder = 1;
	*producers_param = (producer_t *)malloc(sizeof(producer_t) * 
												num_of_producers);
	file_segments_arr = malloc(size_of_pointer * (num_of_segments + remainder));
	g_bucket_histo = CBufferCreate(size_of_pointer * num_of_producers); 
	*producers_id = (pthread_t *)malloc(sizeof(pthread_t) * num_of_producers);
	
	if (NULL == producers_param || NULL == file_segments_arr ||
		NULL == g_bucket_histo || NULL == producers_id)
	{
		DestroyAll(num_of_producers, num_of_segments, 
					*producers_param, *producers_id);
		exit (EXIT_FAILURE);
	}
}

static void PrintTotalHistogream()
{
	int i = 0;
	char ch = 'a';

	for(i = 0; i < NUM_OF_LETTERS; ++i)
	{
		printf("%c: %d\n", ch, g_total_histo[i]);
		++ch;
	}
}

static void *CountCharInSegment(void *param) /* producer */
{
	int assert_status = 0;
	unsigned char *segment_ptr = NULL;
	producer_t *param_for_prod = (producer_t *)param;
	size_t wrote_bytes = 0;

	while(1)
	{
		assert_status = pthread_mutex_lock (&g_lock_segments_arr);
		assert(!assert_status);

		++g_i_segment_arr;
		if (param_for_prod->num_of_segments < g_i_segment_arr) 
		{
			wrote_bytes = CBufferWrite(g_bucket_histo, 
						&(param_for_prod->personal_histo), size_of_pointer);
			assert (size_of_pointer == wrote_bytes);

			if(failed_status == sem_post(&g_histo_to_read))
		    {
		    	perror("sem_post faild: ");
		    	exit(EXIT_FAILURE);
	    	}
	    	assert_status = pthread_mutex_unlock(&g_lock_segments_arr);
			assert(!assert_status);

			return (NULL);
		}

		segment_ptr = file_segments_arr[g_i_segment_arr];
		assert_status = pthread_mutex_unlock(&g_lock_segments_arr);
		assert(!assert_status);

		FillLocalHistogram(segment_ptr, param_for_prod->personal_histo);
	}

	return (NULL);
}

static void FillLocalHistogram(unsigned char *segment_ptr, int *histo_ptr)
{
	unsigned char current_char = 0;
	int i = 0;

	for (i = 0; i < g_size_of_segment ;++i)
	{
		current_char = segment_ptr[i];
		if (((current_char >= 'a') && (current_char <= 'z')) ||
			((current_char >= 'A') && (current_char <= 'Z')))
		{
			current_char = tolower(current_char);
			++histo_ptr[current_char - 'a'];
		}
	}

	return;
}

static int JoinThreads(int num_of_threads, pthread_t *thread_id)
{
	int i = 0; 
	
	for (i = 0; i < num_of_threads; ++i)
	{
		if (pthread_join(thread_id[i], NULL))
		{
			printf("pthread join producer failed");

			return (EXIT_FAILURE);
		}		
	}

	return (EXIT_SUCCESS);
}

static void CreateThreads(int num_of_segmants, int num_of_threads, 
				pthread_t *thread_id, void *param, func_ptr func)
{
	int i = 0; 
	producer_t *params = param;
	
	for (i = 0; i < num_of_threads; ++i)
	{
		if (pthread_create(&thread_id[i], NULL, func, &params[i]))
		{
			perror("pthread create failed: ");
			JoinThreads(i, thread_id);
			DestroyAll(num_of_threads, num_of_segmants, params, thread_id);
			exit(EXIT_FAILURE);
		}		
	}

	return;
}

static int MapFileSegments(int num_of_segments, char *filename)
{
	int i = 0;
	int fd = 0;
	int close_status = 0;

	fd = open(filename, O_RDONLY);
	if (failed_status == fd)
	{
		puts("open failed");
		return(EXIT_FAILURE);
	}

	for (i = 0; i <= num_of_segments; ++i)
	{
		file_segments_arr[i] = mmap(NULL, g_size_of_segment, PROT_READ, 
								MAP_PRIVATE, fd, (i * g_size_of_segment));
		if (MAP_FAILED == file_segments_arr[i])
		{
			perror("mmap failed ");
			return (EXIT_FAILURE);
		}
	}
	
	close_status = close(fd);
	if (failed_status == close_status)
	{
		puts("close failed");
		return(EXIT_FAILURE);
	}

	return (EXIT_SUCCESS);
}

static void DestroyAll(int num_of_producers, int num_of_segments,
						producer_t *producers_param, pthread_t *producers_id)
{
	int i = 0;
	int assert_status = 0;

	for (i = 0; i < num_of_producers; ++i)
	{
		free(producers_param[i].personal_histo);
		producers_param[i].personal_histo = NULL;
	}

	for (i = 0; i <= num_of_segments; ++i)
	{
		munmap(file_segments_arr[i], g_size_of_segment);
	}

	free(producers_param);
	free(producers_id);
	free(file_segments_arr);
	CBufferDestroy(g_bucket_histo);
	assert_status = pthread_mutex_destroy(&g_lock_segments_arr);
	assert(!assert_status);
	assert_status = pthread_mutex_destroy(&g_lock_buffer);	
	assert(!assert_status);
	assert_status = sem_destroy(&g_histo_to_read);
	assert(!assert_status);
	producers_param = NULL;
	producers_id = NULL;
	file_segments_arr = NULL;
	g_bucket_histo = NULL;
}

static void *CalcSumOfHisto(void *param) /*  consumer */
{
	int num_of_producers = *(int *)param;
	size_t read_bytes = 0;
	int i = 0, k = 0;
	int *current_local_hist = calloc(NUM_OF_LETTERS, sizeof(int));
	int *calloc_to_free = current_local_hist;
	if (NULL == current_local_hist)
	{
		return (NULL);
	}

	for(i = 0; i < num_of_producers; ++i)
	{
		if (EXIT_SUCCESS != sem_wait(&g_histo_to_read))
		{
			perror("sem_wait failed: ");
    		
    		return (NULL);
		}
		pthread_mutex_lock(&g_lock_buffer);
		
		read_bytes = CBufferRead(g_bucket_histo, &current_local_hist, 
													size_of_pointer);
		assert (size_of_pointer == read_bytes);

		pthread_mutex_unlock(&g_lock_buffer);
		
		for (k = 0; k < NUM_OF_LETTERS; ++k)
		{
			g_total_histo[k] += current_local_hist[k];
		} 	
	}

	free(calloc_to_free);
	calloc_to_free = NULL;

	return (NULL);
}

static void InitProducerParam(producer_t *producers_param, int num_of_producers,
							 int num_of_segments, pthread_t *producers_id)
{
	int i = 0;

	assert(producers_param);

	for (i = 0; i < num_of_producers; ++i)
	{
/*		producers_param[i].lock_segments_arr = PTHREAD_MUTEX_INITIALIZER;
*/		producers_param[i].num_of_segments = num_of_segments;
		producers_param[i].personal_histo = calloc(NUM_OF_LETTERS, sizeof(int));
		if (NULL == producers_param[i].personal_histo)
		{
			DestroyAll(num_of_segments, num_of_producers, 
							producers_param, producers_id);
			exit(EXIT_FAILURE);
		}
	}

	return;
}