#include <assert.h> /* assert */
#include <errno.h> /* perror */
#include <limits.h> /* MAX_INT */
#include <pthread.h> /*pthread_create, pthread_join, pthread_t */
#include <stdio.h> /* fopen */
#include <stdlib.h> /* malloc, free */
#include <string.h> /* strlen */

#include <sys/stat.h> /* stat */
#include <sys/types.h> /* stat */
#include <unistd.h> /* close */

#define RETURN_FAILURE(x) if (FAIL == (x)){return (EXIT_FAILURE);}
#define DESTROY_AND_RETURN_FAIL(x, a, b, c, d) if(EXIT_FAILURE == x)\
                        {Destroy(a, b, c, d);return(EXIT_FAILURE);}

const int FAILED = -1;

typedef void *(*func_ptr)(void *);

typedef enum boolian_s {
    TRUE = 1, 
    FALSE = 0
} boolian_t;

typedef enum status_s {
    SUCCESS = 0, 
    FAIL = 1
} status_t;

typedef struct thread_params_s
{
    char **words_ptr;
    size_t from;
    size_t to;
} thread_params_t;

static void Destroy(char *cpy_of_file, char **ptr_words, pthread_t *threads_id,
                                        thread_params_t *threads_param);
static void InitParamThreads(char **words_ptr, int lines,
                                thread_params_t *threads_param,
                                size_t num_of_threads);
static void CpyMergedStrToSrc(char **dest, char **src, int size_merge);
static void InitWordsPtrArr(char **words_ptr, char *cpy_of_file, int lines);
static void QuickSort(char **base, size_t num_elements);
static void *QuickSortFunc(void *param);
static void Swap(char **x, char **y);
static size_t FileSize(char *file_name);
static boolian_t IsBefore(char *str1, char *str2);
static status_t InitParameters(int argc, char *argv[], char **output_file,
                        char **input_file, int *num_of_threads);
static status_t AllocThreads(pthread_t **threads_id, 
                            thread_params_t **threads_param,
                            int num_of_threads);
static status_t AllocWordsPtrArray(char ***words_ptr, int lines);
static status_t AllocCpyOfFile(char **cpy_of_file, size_t file_size);
static status_t WriteFileToArray(char *file_name, char *cpy_of_file,
                                        size_t file_size,int *lines);
static status_t JoinThreads(int num_of_threads, pthread_t *thread_id);
static status_t CreateThreads(int num_of_threads, pthread_t *thread_id,
                                            void *param, func_ptr func);
static status_t MergeConv(char **left_arr, int left_arr_size, 
                        char **right_arr, int right_arr_size);
static status_t Merge(thread_params_t *threads_param, 
                int num_of_threads, char **words_ptr);
static status_t WriteArrayToFile(char *file_name, char **cpy_to_file, 
                                                        size_t lines);

int main(int argc, char *argv[])
{
    thread_params_t *threads_param = NULL;
    pthread_t *threads_id = NULL;
    char **words_ptr = NULL;
    char *output_file = NULL;
    char *input_file = NULL;
    char *cpy_of_file = NULL;
    int num_of_threads = 0;
    size_t file_size = 0;
    int lines = 1;

    RETURN_FAILURE(InitParameters(argc, argv, &output_file, &input_file, 
                                                    &num_of_threads));
    file_size = FileSize(output_file);
    RETURN_FAILURE(file_size);

    RETURN_FAILURE(AllocCpyOfFile(&cpy_of_file, file_size));

    DESTROY_AND_RETURN_FAIL(WriteFileToArray(output_file, cpy_of_file, 
                                                    file_size, &lines), 
                    cpy_of_file, words_ptr, threads_id, threads_param);

    DESTROY_AND_RETURN_FAIL(AllocWordsPtrArray(&words_ptr, lines), 
                cpy_of_file, words_ptr, threads_id, threads_param);

     num_of_threads %= lines;

    InitWordsPtrArr(words_ptr, cpy_of_file, lines);

    DESTROY_AND_RETURN_FAIL(WriteArrayToFile(input_file, words_ptr, lines), 
                          cpy_of_file, words_ptr, threads_id, threads_param);

    DESTROY_AND_RETURN_FAIL(AllocThreads(&threads_id, &threads_param, 
                                                    num_of_threads), 
                cpy_of_file, words_ptr, threads_id, threads_param);

    InitParamThreads(words_ptr, lines,threads_param, num_of_threads);

    DESTROY_AND_RETURN_FAIL(CreateThreads(num_of_threads, threads_id, 
                                        threads_param,QuickSortFunc), 
                    cpy_of_file, words_ptr, threads_id, threads_param);

    DESTROY_AND_RETURN_FAIL(JoinThreads(num_of_threads, threads_id), 
                cpy_of_file, words_ptr, threads_id, threads_param);

    DESTROY_AND_RETURN_FAIL(Merge(threads_param, num_of_threads, words_ptr), 
                        cpy_of_file, words_ptr, threads_id, threads_param);

    DESTROY_AND_RETURN_FAIL(WriteArrayToFile(input_file, words_ptr, lines), 
                        cpy_of_file, words_ptr, threads_id, threads_param);

    Destroy(cpy_of_file, words_ptr, threads_id, threads_param);
    
    return (EXIT_SUCCESS);
}

static status_t InitParameters(int argc, char *argv[], char **output_file,
                        char **input_file, int *num_of_threads)
{
    if(argc < 4)
    {
        puts("add number of threads and files name");
        
        return (FAIL);
    }

    *output_file = argv[1];
    *input_file = argv[2];
    *num_of_threads = atoi(argv[3]);

    if (*num_of_threads < 1)
    {
        puts("add a number of threads");
        
        return (FAIL);
    }

    return (SUCCESS);
}

static status_t AllocThreads(pthread_t **threads_id, 
                            thread_params_t **threads_param,
                            int num_of_threads)
{
    *threads_param = (thread_params_t *)malloc(sizeof(thread_params_t) * num_of_threads);
	*threads_id = (pthread_t *)malloc(sizeof(pthread_t) * num_of_threads);

	if (NULL == threads_param || NULL == threads_id)
	{
        perror("malloc failed ");
        
        return (FAIL);
	}

    return (SUCCESS);
}

static status_t AllocWordsPtrArray(char ***words_ptr, int lines)
{
    *words_ptr = (char **)malloc(sizeof(char *) * lines);
    if (NULL == *words_ptr)
    { 
        return (FAIL);
    }

    return (SUCCESS);
}

static status_t AllocCpyOfFile(char **cpy_of_file, size_t file_size)
{
    *cpy_of_file = (char *)malloc(file_size * sizeof(char) + 1);
    if (NULL == *cpy_of_file)
    {
        perror("malloc failed ");
        
        return (FAIL);
    }

    return (SUCCESS);
}

static void Destroy(char *cpy_of_file, char **ptr_words, pthread_t *threads_id,
                                        thread_params_t *threads_param)
{
    free(cpy_of_file);
    free(ptr_words);
    free(threads_id);
    free(threads_param);
    cpy_of_file = NULL;
    ptr_words = NULL;
    threads_id = NULL;
    threads_param = NULL;
}

static void InitWordsPtrArr(char **words_ptr, char *cpy_of_file, int lines)
{
    int next_word = 0, k = 0;

    for (k = 0; k < lines; ++k)
    {
        words_ptr[k] = cpy_of_file + next_word;
        next_word += (strlen(cpy_of_file + next_word) + 1);
    }

    return;
}

static size_t FileSize(char *file_name)
{
    size_t file_size = 0;
    struct stat st;

    if (FAILED == stat(file_name, &st))
    {
        printf("stat failed, filename: %s. error is: %s", file_name, strerror(errno));
        
        return (FAIL);
    }

    file_size = st.st_size;

    return (file_size);
}

static status_t WriteFileToArray(char *file_name, 
                            char *cpy_of_file,
                            size_t file_size,
                            int *lines)
{
    char ch = 0;
    size_t i = 0;
    int status_fread = 0;
    int const num_to_read_char_to_file = 1;
    FILE *fp = fopen(file_name ,"r");

    if (NULL == fp)
    {
       printf("fopen failed, filename: %s. error is: %s", file_name, strerror(errno));

       return (FAIL);
    }

    assert(NULL != cpy_of_file);

    for (i = 0; i < file_size; ++i)
    {
       status_fread = fread(&ch, sizeof(char), num_to_read_char_to_file, fp);
       if (num_to_read_char_to_file != status_fread)
       {
           puts("fread failed ");
           free(cpy_of_file);
           cpy_of_file = NULL;
           
           return (FAIL);
       }
        if ('\n' == ch)
        {
            ++(*lines);
            ch = '\0';
        }
       cpy_of_file[i] = ch;
    }
    cpy_of_file[i] = '\0';

    if (FAILED == fclose(fp))
    {
        printf("fclose failed, filename: %s. error is: %s", file_name, strerror(errno));
        free(cpy_of_file);
        cpy_of_file = NULL;
        
        return (FAIL);
    }

    return (SUCCESS);
}

static void InitParamThreads(char **words_ptr, int lines,
                                thread_params_t *threads_param,
                                size_t num_of_threads)
{
	size_t i = 0;
    const int REMAINDER = 1;
    size_t words_ptr_size = lines;
    int range = (words_ptr_size / num_of_threads);

	assert(words_ptr);

	for (i = 0; i < num_of_threads; ++i)
	{
        threads_param[i].words_ptr = words_ptr;
		threads_param[i].from = i * range; 
        threads_param[i].to = threads_param[i].from + range - REMAINDER;
    }

    --i;
    threads_param[i].to = words_ptr_size - REMAINDER;

	return;
}

static void Swap(char **x, char **y)
{
    char *tmp = *x;
    *x = *y;
    *y = tmp;

    return;
}

static boolian_t IsBefore(char *str1, char *str2)
{
    if (strcmp(str1, str2) < 0)
    {
        return (TRUE);
    }

    return (FALSE);
}

static void QuickSort(char **base, size_t num_elements)
{
	int i = -1;
	size_t j = 0;
    size_t pivot = num_elements - 1;
	size_t left_arr_size = 0, right_arr_size = 0;

	if (1 >= num_elements)
	{
		return;
	}

	for (j = 0 ;j < num_elements; ++j)
	{
        if(IsBefore(base[j], base[pivot]))
        {
            ++i;
            Swap(&base[j], &base[i]);
        }
    }

    ++i;
    Swap(&base[pivot], &base[i]);

	left_arr_size = i;
	right_arr_size = num_elements - i - 1;

	QuickSort(base, left_arr_size);
	QuickSort(&base[i + 1], right_arr_size);
}

static void *QuickSortFunc(void *param)
{
    size_t num_elements = ((thread_params_t *)param)->to - ((thread_params_t *)param)->from;
    char **words_ptr = ((thread_params_t *)param)->words_ptr;
    size_t index = ((thread_params_t *)param)->from;

    QuickSort(&words_ptr[index], num_elements + 1);

    return (NULL);
}

static status_t JoinThreads(int num_of_threads, pthread_t *thread_id)
{
	int i = 0;

	for (i = 0; i < num_of_threads; ++i)
	{
		if (pthread_join(thread_id[i], NULL))
		{
			perror("pthread join failed ");

            return (FAIL);
		}
	}

    return (SUCCESS);
}

static status_t CreateThreads(int num_of_threads, pthread_t *thread_id,
                                            void *param, func_ptr func)
{
	int i = 0;
	thread_params_t *params_for_threads = param;

	for (i = 0; i < num_of_threads; ++i)
	{
		if (0 != pthread_create(&thread_id[i], NULL, func, &params_for_threads[i]))
		{
			perror("pthread create failed: ");
            JoinThreads(num_of_threads, thread_id);

            return (FAIL);
		}
	}

	return (SUCCESS);
}

static void CpyMergedStrToSrc(char **dest, char **src, int size_merge)
{
	int i;

	for (i = 0; i < size_merge; ++i)
	{
		dest[i] = src[i];
	}
}

static status_t MergeConv(char **left_arr, int left_arr_size, char **right_arr, int right_arr_size)
{
	int i_left = 0, i_right = 0, i_merge = 0;
	int size_merge = left_arr_size + right_arr_size;

	char **merge_arr = (char **)malloc(sizeof(char *) * (size_merge));
	if (NULL == merge_arr)
	{
        perror("malloc failed ");

		return (FAIL);
	}

	for ( ;(i_right < right_arr_size) && (i_left < left_arr_size); ++i_merge)
	{
        if (IsBefore(left_arr[i_left], right_arr[i_right]))
		{
			merge_arr[i_merge] = left_arr[i_left];
			++i_left;
		}
		else
		{
			merge_arr[i_merge] = right_arr[i_right];
			++i_right;
		}
	}

	for (; (i_right < right_arr_size); ++i_right)
	{
		merge_arr[i_merge] = right_arr[i_right];
		++i_merge;
	}

	for (; (i_left < left_arr_size); ++i_left)
	{
		merge_arr[i_merge] = left_arr[i_left];
		++i_merge;
	}

    CpyMergedStrToSrc(left_arr, merge_arr, size_merge);
	free(merge_arr);
	merge_arr = NULL;

    return (SUCCESS);
}

static status_t Merge(thread_params_t *threads_param, int num_of_threads, char **words_ptr)
{
    int i = 0;
    int left_arr_size = 0, right_arr_size = 0;

    for(i = 1; i < num_of_threads; ++i)
    {
        left_arr_size = threads_param[i - 1].to - threads_param[0].from + 1;
        right_arr_size = threads_param[i].to - threads_param[i].from + 1;

        if (FAIL == MergeConv(&words_ptr[0], left_arr_size,
                    &words_ptr[threads_param[i].from], right_arr_size))
        {
            return (FAIL);
        }
    }

    return (SUCCESS);
}

static status_t WriteArrayToFile(char *file_name, char **cpy_to_file, size_t lines)
{
    size_t i = 0;
    const char ENTER = '\n';
    FILE *fp = fopen(file_name ,"w");

    if (NULL == fp)
    {
       printf("fopen failed, filename: %s. error is: %s", file_name, strerror(errno));
        return (FAIL);
    }

    for (i = 0; i < lines; ++i)
    {
        if (EOF == fputs(cpy_to_file[i], fp))
        {
            puts("fputs failed\n");

            return (FAIL);
        }

        if (ENTER != fputc(ENTER, fp))
        {

            puts("fputc failed\n");

            return (FAIL);
        }
    }

    if (-1 == fclose(fp))
    {
        printf("fclose failed, filename: %s. error is: %s", file_name, strerror(errno));
        
        return (FAIL);
    }

    return (SUCCESS);
}
	
