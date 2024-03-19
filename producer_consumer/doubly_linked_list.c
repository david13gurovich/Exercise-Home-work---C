#include <stdlib.h> /* size_t, malloc, free */
#include <assert.h> 

#include "ol62_general.h"
#include "doubly_linked_list.h"

#define dummy_data ((void*)(0xDEADBEEF))

typedef struct dll_node_s
{
	struct dll_node_s *next;
	struct dll_node_s *pre;
	void *data;
} dll_node_t;

struct dll_s
{
	dll_node_t *head;
	dll_node_t *tail;
};

static dll_node_t *CreateNodeDll(dll_node_t *next, dll_node_t *pre, void *data)
{
	dll_node_t *new_node = (dll_node_t *)malloc(sizeof(dll_node_t));
	if (NULL == new_node)
	{
		return (NULL);
	}

	new_node->pre = pre;
	new_node->next = next;
	new_node->data = data;

	return (new_node);
}

dll_t *DLLCreate()
{
	dll_t *dll_control;
	dll_node_t *dummy_head;
	dll_node_t *dummy_tail;

	dll_control = (dll_t *)malloc(sizeof(dll_t));
	if (NULL == dll_control)
	{
		return (NULL);
	}

	dummy_head = CreateNodeDll(NULL, NULL, dummy_data);
	if (NULL == dummy_head)
	{
		free(dll_control);
		dll_control = NULL;
		return (NULL);
	}

	dummy_tail = CreateNodeDll(NULL, dummy_head, dummy_data);
	if (NULL == dummy_tail)
	{
		free(dll_control);
		free(dummy_head);
		dll_control = NULL;
		dummy_head = NULL;
		return (NULL);
	}

	dummy_head->next = dummy_tail;

	dll_control->head = dummy_head;
	dll_control->tail = dummy_tail;

	return (dll_control);
}

void DLLDestroy(dll_t *list)
{
	dll_node_t *node_to_free; 
	dll_node_t *node_to_free_next;

	assert(NULL != list);

	node_to_free = list->head;
	while(NULL != node_to_free)
	{
		node_to_free_next = node_to_free->next;
		free(node_to_free);
		node_to_free = node_to_free_next;
	}

	free(list);
	list = NULL;

	return;
}

boolean_t DLLIsEmpty(const dll_t *list)
{
	assert(NULL != list);

	return (list->head->next == list->tail);
}

size_t DLLGetSize(const dll_t *list)
{
	dll_node_t *run = (dll_node_t *)list->head->next;
	size_t cnt = 0;

	while (NULL != run->next)
	{
		++cnt;
		run = run->next;
	}

	return (cnt);
}

static dll_iterator_t InsertToList(dll_t *list, dll_iterator_t where, void *data)
{
	dll_iterator_t return_val;
	dll_node_t *new_node;
	dll_node_t *where_to_insert = (dll_node_t *)where.info;

	assert(NULL != list);
	assert(NULL != where_to_insert);

	if (where.info == list->head)
	{
		return_val.info = list->tail;
		return (return_val);
	}

	new_node = CreateNodeDll(where_to_insert, where_to_insert->pre, data);
	if (NULL == new_node)
	{
		where.info = NULL;
		return (where);
	}

	(where_to_insert->pre)->next = new_node;
	where_to_insert->pre = new_node;

	return_val.info = new_node;

	return (return_val);
}

dll_iterator_t DLLInsert(dll_t *list, dll_iterator_t where, void *data)
{	
	assert(NULL != list);
	assert(FALSE == DLLIsEmpty(list));

	return (InsertToList(list, where, data));
}

dll_iterator_t DLLPushFront(dll_t *list, void *data)
{
	dll_iterator_t where;
	assert(NULL != list);

	where.info = list->head->next;

	return (InsertToList(list, where, data));	
}

dll_iterator_t DLLPushBack(dll_t *list, void *data)
{
	dll_iterator_t where;
	assert(NULL != list);
	
	where.info = list->tail;

	return (InsertToList(list, where, data));
}

void *DLLGetData(const dll_iterator_t iterator)
{
	assert(NULL != iterator.info);
	
	return (((dll_node_t *)(iterator.info))->data);
}

dll_iterator_t DLLGetNext(const dll_iterator_t iterator)
{
	dll_iterator_t return_val;
	assert(NULL != iterator.info);

	return_val.info = ((dll_node_t *)(iterator.info))->next;

	return (return_val);
}

dll_iterator_t DLLGetPrevious(const dll_iterator_t iterator)
{
	dll_iterator_t return_val;
	assert(NULL != iterator.info);

	return_val.info = ((dll_node_t *)(iterator.info))->pre;

	return (return_val);
}

dll_iterator_t DLLBegin(const dll_t *list)
{
	dll_iterator_t return_val;
	assert(NULL != list);

	return_val.info = list->head->next;
	
	return (return_val);
}

dll_iterator_t DLLEnd(const dll_t *list)
{
	dll_iterator_t return_val;
	assert(NULL != list);

	return_val.info = list->tail;
	
	return (return_val);
}

dll_iterator_t DLLErase(dll_iterator_t iterator)
{
	dll_iterator_t return_val;
	dll_node_t *to_removed = iterator.info;
	assert(NULL != to_removed);

	return_val.info = to_removed->next;

	to_removed->pre->next = to_removed->next;
	to_removed->next->pre = to_removed->pre;

	to_removed->next = NULL;
	to_removed->pre = NULL;
	free(to_removed);
	to_removed = NULL;

	return (return_val);
}

void DLLPopFront(dll_t *list)
{
	dll_iterator_t first_node;
	assert(NULL != list);

	first_node.info = list->head->next;

	DLLErase(first_node);

	return;
}

void DLLPopBack(dll_t *list)
{
	dll_iterator_t last_node;
	assert(NULL != list);

	last_node.info = list->tail->pre;

	DLLErase(last_node);

	return;
}

boolean_t DLLIsSameIterator(const dll_iterator_t iterator1, const dll_iterator_t iterator2)
{
	return (iterator1.info == iterator2.info);
}

dll_iterator_t DLLFind(dll_t *list,const dll_iterator_t from, const dll_iterator_t to, 
					   is_match_t is_match, void *data_for_cmp, void *param)
{
	dll_iterator_t runner = from;

	while (runner.info != to.info)
	{
		if (is_match(((dll_node_t *)(runner.info))->data, data_for_cmp, param))
		{
			return (runner);
		}
		
		runner.info = ((dll_node_t *)(runner.info))->next;
	}

	return (DLLEnd(list));
}

status_t DLLForEach(dll_iterator_t from, dll_iterator_t to, action_t action, void *param)
{
	dll_iterator_t runner = from;
	assert(NULL != runner.info);
	assert(NULL != to.info);		

	while (runner.info != to.info)
	{
		if (FAIL == action(((dll_node_t *)(runner.info))->data, param))
		{
			return (FAIL);
		}

		runner.info = ((dll_node_t *)(runner.info))->next;
	}
	
	return (SUCCESS);
}

void DLLSplice(dll_iterator_t where, dll_iterator_t from, dll_iterator_t to)
{
	dll_node_t *before_where;
	dll_node_t *to_pre;

	assert(where.info);
	assert(from.info);
	assert(to.info);

	before_where = ((dll_node_t *)where.info)->pre;
	to_pre = ((dll_node_t *)to.info)->pre;

	((dll_node_t *)from.info)->pre->next = to.info;
	((dll_node_t *)to.info)->pre = ((dll_node_t *)from.info)->pre;

	before_where->next = from.info;
	((dll_node_t *)from.info)->pre = before_where;

	to_pre->next = where.info;
	((dll_node_t *)where.info)->pre = to_pre;
}