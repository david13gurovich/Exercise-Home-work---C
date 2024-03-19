#include <stdlib.h> /* malloc, free, exit_status */ 
#include <pthread.h> /* pthread_create, join */
#include <stdio.h> /* printf */

const size_t to_check = 99999999;
static size_t *g_sum_of_div = NULL;

typedef struct thread_data
{
	size_t index;
	size_t start;
	size_t end;
} thread_data_t;

static void HandleThreads(size_t num_thread,thread_data_t *param, 
						 pthread_t *threads, size_t range);
static void Destroy(pthread_t *threads, thread_data_t *param);
static void *SumOfDiv(void *thread_data);

int main(int argc, char const *argv[])
{
	size_t i = 0;
	size_t ans = 0;
	size_t num_thread = 0; 
	size_t range_to_divide = 0; 

	pthread_t *threads;
	thread_data_t *param_to_func = NULL; 
	
	if (argc < 2)
	{
		puts ("\nplease add the argumant: number ot threads");

		return (EXIT_FAILURE);
	}

	num_thread = atoi(argv[1]);
	range_to_divide = to_check / num_thread;
	g_sum_of_div = (size_t *)malloc(num_thread * sizeof(size_t));
	threads = (pthread_t *)malloc(num_thread * sizeof(pthread_t));
	param_to_func = (thread_data_t *)malloc(num_thread * sizeof(thread_data_t));
	
	if (NULL == param_to_func || NULL == threads || NULL == g_sum_of_div)
	{
		Destroy(threads, param_to_func);
		param_to_func = NULL;
		threads = NULL;	
		g_sum_of_div = NULL;

		return (EXIT_FAILURE);
	}

	HandleThreads(num_thread, param_to_func, threads, range_to_divide);

	for (i = 0; i < num_thread; ++i)
	{
		ans += g_sum_of_div[i];
	}

	printf("%lu\n", ans);

	Destroy(threads, param_to_func);
	param_to_func = NULL;
	threads = NULL;	
	g_sum_of_div = NULL;

	return (EXIT_SUCCESS);
}

static void *SumOfDiv(void *thread_data)
{
	size_t i = 0;
	size_t sum = 0;

#pragma omp parallel for
	for (i = ((thread_data_t *)thread_data)->start + 1; 
		 i < ((thread_data_t *)thread_data)->end + 1; 
		 ++i)
	{
		if (0 == to_check % i)
		{
			sum += i;
		}
	}

	g_sum_of_div[((thread_data_t *)thread_data)->index] = sum;

	return (NULL);
}

static void HandleThreads(size_t num_thread, 
						 thread_data_t *param, 
						 pthread_t *threads, 
						 size_t range)
{
	size_t start_range = 0;
	size_t i = 0;

	for (i = 0; i < num_thread; ++i)
	{
		param[i].index = i;
		param[i].start = start_range;
		param[i].end = start_range + range;
		if (i == (num_thread - 1)) 
		{
			param[i].end = to_check;
		}
		start_range += range;
		if (EXIT_SUCCESS != pthread_create (&threads[i], NULL, &SumOfDiv, &param[i]))
		{
			perror("Create threads failed:");
			exit(EXIT_FAILURE);
		}	
	}

	for (i = 0; i < num_thread; ++i)
	{
		if (EXIT_SUCCESS != pthread_join (threads[i], NULL))
		{
			puts("pthread_join failed:");
			exit(EXIT_FAILURE);
		}
	}

	return;
}

static void Destroy(pthread_t *threads, thread_data_t *param_to_func)
{
	free(param_to_func);
	free(threads);
	free(g_sum_of_div);

	param_to_func = NULL;
	threads = NULL;	
	g_sum_of_div = NULL;

	return;
}

