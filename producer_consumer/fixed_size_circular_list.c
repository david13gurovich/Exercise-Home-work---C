#include <string.h> /*memcpy*/ 
#include <assert.h>
#include <stdlib.h> /* malloc size_t free */

#include "fixed_size_circular_list.h"

struct CList_s {
	char *base;
	char *read;
	char *write;
	size_t capacity;
	size_t number_of_unread_bytes;
};

CList_t *CListCreate(size_t capacity)
{
	CList_t *new_buffer = malloc(sizeof(CList_t));
	if(NULL == new_buffer)
	{
		return (NULL);
	}

	new_buffer->base = malloc(capacity);
	if(NULL == new_buffer->base)
	{
		free(new_buffer);
		return (NULL);
	}
	new_buffer->read = new_buffer->base;
	new_buffer->write = new_buffer->base;
	new_buffer->capacity = capacity;
	new_buffer->number_of_unread_bytes = 0;

	return (new_buffer);
}

void CListDestroy(CList_t *CList)
{
	assert(NULL != CList);
	
	free(CList->base);
	CList->base = NULL;
	free(CList);
	CList = NULL; 
}

size_t CListWrite(CList_t *CList, const void *src, size_t num_bytes_to_write)
{
	size_t num_free_bytes = 0;
	size_t byts_left_over = 0;
	size_t num_byts_write_until_end = 0;
	
	assert(NULL != CList);
	assert (NULL != src); 
	
	num_free_bytes = CList->capacity - CList->number_of_unread_bytes;
	num_bytes_to_write = (num_free_bytes >= num_bytes_to_write) ? 
								num_bytes_to_write :  num_free_bytes;
	
	if(CList->write + num_bytes_to_write <= CList->base + CList->capacity)
	{
		memcpy(CList->write, src, num_bytes_to_write);
		CList->write += num_bytes_to_write;
	}
	else
	{
		byts_left_over = num_bytes_to_write -
		((CList->base + CList->capacity) - CList->write);

		num_byts_write_until_end = (num_bytes_to_write - byts_left_over);
		memcpy(CList->write, src, num_byts_write_until_end);
		
		CList->write = CList->base;
		memcpy(CList->write, (char *)src + num_byts_write_until_end, byts_left_over);
		CList->write += byts_left_over; 
	}
	
	CList->number_of_unread_bytes += num_bytes_to_write;
	
	return (num_bytes_to_write);
}

size_t CListRead(CList_t *CList, void *dest, size_t numb_bytes_to_read)
{
	size_t byts_left_over;
	size_t num_byts_read_until_end_buffer;
	
	assert(NULL != CList);
	assert (NULL != dest);

	numb_bytes_to_read = (numb_bytes_to_read <= CList->number_of_unread_bytes) ?
							numb_bytes_to_read : CList->number_of_unread_bytes;

	if (CList->read + numb_bytes_to_read <= CList->base + CList->capacity)
	{
		memcpy(dest, CList->read, numb_bytes_to_read);
		CList->read += numb_bytes_to_read;
	}
	else
	{
		
		num_byts_read_until_end_buffer = (CList->base + CList->capacity) - CList->read;
		byts_left_over = numb_bytes_to_read - num_byts_read_until_end_buffer;

		memcpy(dest, CList->read, num_byts_read_until_end_buffer);
		CList->read = CList->base;
		memcpy((char *)dest + num_byts_read_until_end_buffer, CList->read, byts_left_over);
		CList->read += byts_left_over;

	}
	
	CList->number_of_unread_bytes -= numb_bytes_to_read;

	return (numb_bytes_to_read);
}

int CListIsEmpty(const CList_t *CList)
{
	assert(NULL != CList);

	return(!(CList->number_of_unread_bytes));
}

size_t CListGetCapacity(const CList_t *CList)
{
	assert(NULL != CList);

	return (CList->capacity);
}

size_t CListGetSize(const CList_t *CList)
{
	assert(NULL != CList);

	return (CList->number_of_unread_bytes);
}

void CListClear(CList_t *CList)
{
	assert(NULL != CList);

	CList->read = CList->base;
	CList->write = CList->base;
	CList->number_of_unread_bytes = 0;
}