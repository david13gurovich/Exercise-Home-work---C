#include <stdlib.h> /* malloc, free, exit_status */ 
#include <pthread.h> /* pthread_create */ /* eee join*/
#include <stdio.h> /* printf */
#include <unistd.h> /* sleep *//* eee ?*/
#include <errno.h> /* perror */

#define SIZE (100000)/* eee not used*/

const int success = 0; /* eee same */

/* int arr[SIZE];

void *SetIndex(void *i)
{
	arr[(int)i] = (int)i + 1;

	return (NULL); 
}

int main()
{
	int i = 0;
	int cnt = 0;
	pthread_t thread_id = {0};
	pthread_attr_t attr = {0};
	
	for (i = 0; i < SIZE; ++i)
	{
		if (sussece != pthread_attr_init (&attr))
		{
			perror(NULL);
		}
		if (sussece != pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED))
		{
			perror(NULL);
		}
		if (sussece != pthread_create (&thread_id, &attr, &SetIndex, (void *)i))
		{
			perror(NULL);
		}
		if (sussece != pthread_attr_destroy (&attr))
		{
			perror(NULL);
		}
	}
	
	sleep(10);

	for (i = 0; i < SIZE; ++i)
	{
		if (arr[i] == (i + 1))
		{
			++cnt;
		}
	}

	printf("Number of threads that susseced to write on the array: %d\n", cnt);

	return (0); 
}
*/

const size_t to_check = 9999999999;
static size_t *arr = NULL;/* better name eee */

typedef struct thread_data
{
	size_t index;
	size_t start;
	size_t end;
} thread_data_t;

static void *SumOfDiv(void *thread_data)/* eee not only thread_data. set a better name*/
{
	size_t i = 0;
	size_t sum = 0;

	for (i = ((thread_data_t *)thread_data)->start + 1; 
		 i < ((thread_data_t *)thread_data)->end + 1; 
		 ++i)
	{
		if (0 == to_check % i)
		{
			sum += i;
		}
	}

	arr[((thread_data_t *)thread_data)->index] = sum;

	return (NULL);
}
/* handle threads ... ? eee*/
static void ThreadAction(size_t num_thread, thread_data_t *param, pthread_t *threads, size_t range)
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
			param[i].end = to_check;/* eee num to check ?*/
		}
		start_range += range;
		pthread_create (&threads[i], NULL, &SumOfDiv, &param[i]);/* eee check return code*/
	}


	for (i = 0; i < num_thread; ++i)
	{
		pthread_join (threads[i], NULL);/* eee remove blank*/		
		/* eee check return code*/
	}

	return;
}

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
	arr = (size_t *)malloc(num_thread * sizeof(size_t));
	threads = (pthread_t *)malloc(num_thread * sizeof(pthread_t));
	param_to_func = (thread_data_t *)malloc(num_thread * sizeof(thread_data_t));
	
	if (NULL == param_to_func || NULL == threads || NULL == arr)
	{
		free(param_to_func);
		free(threads);
		free(arr);

		param_to_func = NULL;
		threads = NULL;	
		arr = NULL;

		return (EXIT_FAILURE);
	}

	ThreadAction(num_thread, param_to_func, threads, range_to_divide);

	for (i = 0; i < num_thread; ++i)
	{
		ans += arr[i];
	}

	printf("%lu\n", ans);

	free(param_to_func);
	free(threads);
	free(arr);

	param_to_func = NULL;
	threads = NULL;	
	arr = NULL;

	return (EXIT_SUCCESS);
}
