#include <stdlib.h>

#ifndef __H_DLLIST__
#define __H_DLLIST__

#include "ol62_general.h"

/***********************************************************************
* This file handles a Doubly Linked List
* 
* The Doubly Linked List enables the user to create and manage
* A doubly linked list.
*
* Written by: OL62
* Last updated : 6.1.2019
***********************************************************************/

typedef struct dll_s dll_t;

typedef struct dll_iterator_s
{
	void *info;
} dll_iterator_t;

typedef boolean_t(*is_match_t)(const void *data1, const void *data2, void *param);
typedef status_t(*action_t)(void *data, void *param);


dll_t *DLLCreate();

/***********************************************************************
* DLLCreate - Creates a Doubly Linked List data structure.
*
* Returns - a pointer to the Doubly Linked List. Cannot be NULL.
*
***********************************************************************/

void DLLDestroy(dll_t *list);

/***********************************************************************
* DLLDestroy - Frees the memory allocated for the Doubly Linked List
*
* list - pointer to the Doubly Linked List, not NULL.
*
***********************************************************************/

size_t DLLGetSize(const dll_t *list);

/***********************************************************************
* DLLGetSize - calculates the number of list members.
*
* list - pointer to the Doubly Linked List.
*
* Returns - the number of list members.
*
***********************************************************************/

boolean_t DLLIsEmpty(const dll_t *list);

/***********************************************************************
* DLLIsEmpty - Frees the memory allocated for the Doubly Linked List
*
* list - a pointer to the Doubly Linked List, not NULL.
*
* Returns - boolean_t SUCCESS or FAIL.
***********************************************************************/

dll_iterator_t DLLBegin(const dll_t *list);

/***********************************************************************
* DLLBegin - returns iterator to the beginning of the Doubly Linked
* List.
*
* list - a pointer to the Doubly Linked List, not NULL.
*
* Returns - iterator to the beginning of the Doubly Linked List
***********************************************************************/

dll_iterator_t DLLEnd(const dll_t *list);

/***********************************************************************
* DLLEnd - returns iterator to the end of the Doubly Linked
* List.
*
* list - a pointer to the Doubly Linked List, not NULL.
*
* Returns - iterator to the end of the Doubly Linked List
***********************************************************************/

void *DLLGetData(const dll_iterator_t iterator);

/***********************************************************************
* DLLGetData - returns the data stored in the position provided by
* iterator
*
* iterator - a position in a Doubly Linked List. If iterator is
* not found in the list, the function behaviour is undefined, not iterator.info NULL.
*
* Returns - data in the position specified by iterator.
***********************************************************************/

dll_iterator_t DLLGetNext(const dll_iterator_t iterator);

/***********************************************************************
* DLLGetNext - returns the iterator position to the next member
* in the list.
*
* iterator - a position in a Doubly Linked List. If iterator is
* not found in the list, the function behaviour is undefined. 
* iterator - should not be the last member of the list, not iterator.info NULL.
*
* Returns - an iterator to the next location.
***********************************************************************/

dll_iterator_t DLLGetPrevious(const dll_iterator_t iterator);

/***********************************************************************
* DLLGetPrevious - returns the iterator position to the previous member
* in the list.
*
* iterator - a position in a Doubly Linked List. If iterator is
* not found in the list, the function behaviour is undefined. 
* iterator - should not be the first member of the list, not iterator.info NULL.
*
* Returns - an iterator to the previous location.
***********************************************************************/

boolean_t DLLIsSameIterator(const dll_iterator_t iterator1,
										const dll_iterator_t iterator2);

/***********************************************************************
* DLLIsSameIterator - compares between two iterators, determines
* if they point to the same list members.
*
* iterator1 - a position in a Doubly Linked List.
* iterator2 - a position in a Doubly Linked List.
*
* Returns - boolean_t: TRUE - if iterators point to same member.
* FALSE - if otherwise.
***********************************************************************/

dll_iterator_t DLLInsert(dll_t *list, dll_iterator_t where, void *data);

/***********************************************************************
* DLLInsert - function receives a list and inserts a new member
* which contains data to the location specified.
*
* list - a Doubly Linked List, not NULL and not empty. 
*
* where - a position in a Doubly Linked List beforewhich the new
* member will be inserted.
*
* data - the data the new member will hold.
*
* Returns - iterator - pointing to the position of the new member.
***********************************************************************/

dll_iterator_t DLLPushFront(dll_t *list, void *data);

/***********************************************************************
* DLLPushFront - function receives a list and inserts a new member
* which contains data to the front location.
*
* list - a Doubly Linked List, not NULL.
*
* data - the data the new member will hold.
*
* Returns - iterator - pointing to the position of the new member.
***********************************************************************/

dll_iterator_t DLLPushBack(dll_t *list, void *data);

/***********************************************************************
* DLLPushBack - function receives a list and inserts a new member
* which contains data to the last location.
*
* list - a Doubly Linked List, not NULL.
*
* data - the data the new member will hold.
*
* Returns - iterator - pointing to the position of the new member.
***********************************************************************/

dll_iterator_t DLLErase(dll_iterator_t iterator);

/***********************************************************************
* DLLErase - function receives iterator in a list, and removes
* the member in that position in that list.
*
* iterator - a position to a member in the list, irtrator.info not NULL.
*
* Returns - iterator - pointing to the position of the next member.
* if the last member of a list is removed, the function will return
* an iterator of DLLEnd.
***********************************************************************/

void DLLPopFront(dll_t *list);

/***********************************************************************
* DLLPopFront - function receives a list, and removes
* the member in the front position in that list.
*
* list - a pointer to a list. Cannot be NULL. Cannot be empty list.
*
***********************************************************************/

void DLLPopBack(dll_t *list);

/***********************************************************************
* DLLPopBack - function receives a list, and removes
* the member in the last position in that list.
*
* list - a pointer to a list. Cannot be NULL. Cannot be empty list.
*
***********************************************************************/

dll_iterator_t DLLFind(dll_t *list,const dll_iterator_t from, const dll_iterator_t to,
									is_match_t ismatch, void *data_for_cmp, void *param);

/***********************************************************************
* DLLFind - function receives a pointer list and a range in the list,
* data for comparison and a function for comparison.
* The function will look for the data in the range specified.
*
* list - a pointer to a list. Cannot be NULL. Cannot be empty list. not NULL and not empty
*
* from - an iterator to the member from which the search will be
* started (inclusive). Must be before to in the list order.
*
* to - an interator to the member until which the search will be
* conducted (exclusive). Must be after from in the list order.
*
* ismatch - a function of type is_match_t provided by the user.
*
* data_for_cmp - the data which will be compared to the data in the
* members in the range.
*
* param - an additional parameter that can be utilized by the search
* function.
*
* Returns - iterator. If ismatch function returns true, iterator
* will point to the first member holding the data, in range.
* Otherwise, the function will return an iterator of DLLEnd.
*
***********************************************************************/

status_t DLLForEach(dll_iterator_t from, dll_iterator_t to,
										action_t action, void *param);

/***********************************************************************
* DLLForEach - function receives a range in a Doubly Linked List.
* The function will perform an action for each member in the range.
*
* from - an iterator to the member from which the action will be
* started (inclusive). Must be before to in the list order. not iterator.info NULL.
*
* to - an interator to the member until which the action will be
* conducted (exclusive). Must be after from in the list order. not iterator.info NULL.
*
* action - a function of type action_t provided by the user.
*
* param - an additional parameter that can be utilized by the action
* function.
*
* Returns - type status_t : SUCCESS if action function returned success
* for all members in the range. FAILURE if otherwise. 
***********************************************************************/

void DLLSplice(dll_iterator_t where, dll_iterator_t from, dll_iterator_t to);

/***********************************************************************
* DLLSplice - the function receives a member position in a Doubly
* Linked List and inserts a range of members from another Doubly
* Linked List.
*
* where - an iterator to the position of the member in the list
* beforewhich the range will be inserted. not iterator.info NULL.
*
* from - a member location in the first list from which
* the splicing will be started (inclusive).not iterator.info NULL.
*
* to - a member location in the first list from which the splicing
* will be ended (exclusive).not iterator.info NULL.
*
***********************************************************************/

#endif
