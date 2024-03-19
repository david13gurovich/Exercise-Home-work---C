#include <pthread.h> /* pthread_create */
#include <stdio.h> /* printf */
#include <unistd.h> /* sleep */
#include <errno.h> /* perror */

#define SIZE (100000)

const int success = 0;

int arr[SIZE];

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
		if (success != pthread_attr_init (&attr))
		{
			perror(NULL);
		}
		if (success != pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED))
		{
			perror(NULL);
		}
		if (success != pthread_create (&thread_id, &attr, &SetIndex, (void *)i))
		{
			perror(NULL);
		}
		if (success != pthread_attr_destroy (&attr))
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
