#ifndef __H_CBUFFER_
#define __H_CBUFFER_

#include <stdio.h> /**/

/**************************************************************** 
 * This file handles a Circular Buffer.
 *
 * the Circular Buffer enables the reader to read from the buffer, 
 * and the writer to write to the buffer in a way that the writer 
 * never overwrites data that the reader hasn't already read,  
 * and the reader can only read data that has been written by 
 * the writer. The Circular Buffer recycles the memory that was 
 * already read by the reader, and can re-use it for future writes.
 * 
 *                                                				
 * Written by: 		OL62         								
 * Last updated : 	30.12.2018									 	                                                            *
 ***************************************************************/
typedef struct CList_s CList_t;

CList_t *CListCreate(size_t capacity);
/***************************************************************** 
 * CListrCreate - creates a circular buffer data structure 
 * with a size of 'capacity' bytes. 
 *                                                              
 * capacity - CList size in bytes. 
 *                                                              
 *	Return - a pointer to the CList. NULL if the function was 
 *  unable to create the CList 
 *****************************************************************/

void CListDestroy(CList_t *CList);
/****************************************************************
 * CListrDestroy - Removes the circular buffer from memory            
 *                               
 *                                                              
 * CList - a pointer to the CList with type CList_t. Not NULL.
 ****************************************************************/

size_t CListWrite(CList_t *CList, const void *src, size_t num_bytes_to_write); 
/****************************************************************
 * CListrWrite - Writes num_bytes_to_write from src to buffer. 
 *
 * CList - a pointer to the buffer. Not NULL
 * src - a pointer to the source to be read. Should be at least num_bytes_to_write
 * size. Otherwise, behaviour is undefined.
 * num_bytes_to_write- number of bytes requested to be written.
 * Return - number of bytes succesfully written. 
 ****************************************************************/

size_t CListRead(CList_t *CList, void *dest, size_t num_bytes_to_read);
/************************************************************************
 * CListrRead - Reads num_bytes_to_read from buffer to dest. 
 *
 * CList - a pointer to the buffer. Not NULL.
 * dest - destination pointer to be written. Should be at least bytes_to_read
 * size. Otherwise, behaviour is undefined.  
 * num_bytes_to_read - number of bytes requested to be read.
 * Return - number of bytes succesfully read.
 *************************************************************************/

int CListIsEmpty(const CList_t *CList);
/****************************************************************
 * CListrIsEmpty - checks if the buffer is empty
 *
 * CList - a pointer to the buffer. Not NULL
 * Return - 1 if the buffer is empty, 0 otherwise
 ****************************************************************/

size_t CListGetCapacity(const CList_t *CList);
/****************************************************************
 * CListrGetCapacity - Returns the capacity
 *
 * CList - a pointer to the buffer. Not NULL
 * Return- the capacity                        
 ****************************************************************/

size_t CListGetSize(const CList_t *CList);
/****************************************************************
 * CListrGetSize - calculate the current buffer non-read data size 
 *
 * CList - a pointer to the buffer. Not NULL
 * Return- the current buffer non-read data size 
 ****************************************************************/


void CListClear(CList_t *CList);
/****************************************************************
 * CListrClear - clears the buffer.
 *
 * CList - a pointer to the buffer. Not NULL
 *                              
 * Return- void.
 *******************************************************************/
#endif