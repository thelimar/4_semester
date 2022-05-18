#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

//#define DEBUG
//#define FAULT_INJECTION

#ifndef DEBUG
#define DBG if(0)
#else
#define DBG
#endif


#define list_data_format "%s"

#define strequ(str1, str2) !strcmp(str1, str2)

#ifdef FAULT_INJECTION
static void* FI_calloc(size_t size, size_t size_of)
{
	if (rand() % 10 == 0)
		return NULL;
	return calloc(size, size_of);
}
#else
static void* FI_calloc(size_t size, size_t size_of)
{
	return calloc(size, size_of);
}
#endif

enum ListErrors
{
	SIZE_TOO_BIG = 1,
	SIZE_TOO_SMALL = 2,
	NO_DATA_TO_DELETE = 3,
};

typedef char* list_data;

struct ListNode
{
	list_data data_one;
	list_data data_two;
	struct ListNode* next;
};

struct List
{
	size_t size;
	struct ListNode* head;
	struct ListNode* tail;
};

struct List* ListBuild ()
{

	struct List* dis = (struct List*) FI_calloc (1, sizeof (struct List));
	if (!dis)
		return NULL;

	dis->head = NULL;
	dis->tail = NULL;
	dis->size = 0;

	return dis;
}

struct ListNode* ListNodeBuild (list_data node_data_one, list_data node_data_two)
{
	struct ListNode* node = (struct ListNode*) FI_calloc (1, sizeof (struct ListNode));
	if (node)
	{
		node->data_one = node_data_one;
		node->data_two = node_data_two;
		return node;
	}

		return NULL;
}

void ListNodeDestruct (struct ListNode* dis)
{

	dis->data_one = NULL;
	dis->data_two = NULL;
	free (dis);
}

void ListDestruct (struct List* dis)
{
	struct ListNode* current_node = dis->head;
	struct ListNode* destroy_node = dis->head;

	for (unsigned int i = 0; i < dis->size; i++)
	{
		if (destroy_node->next)
			current_node = destroy_node->next;

		ListNodeDestruct (destroy_node);

		destroy_node = current_node;
	}

	dis->size = 0;
}


int ListPush (struct List* dis, list_data node_data_one, list_data node_data_two)
{
	if (!dis)
		return -1;

	struct ListNode* new_node = ListNodeBuild (node_data_one, node_data_two);

	if (new_node == NULL) 
		return -1;

	if (!dis->size)
	{
		dis->head = new_node;
		dis->tail = new_node;
		dis->size = 1;
	}
	else
	{
		dis->tail->next = new_node;
		(dis->size)++;
		dis->tail = new_node;
	}

	return 0;
}


void ListDump (struct List* dis)
{
	if (!dis)
		return;

	struct ListNode* next_node = dis->head;
	DBG printf("size of list is %ld\r\n", dis->size);
	for (unsigned int i = 0; i < dis->size; i++)
	{
		if (next_node)
		{
			DBG printf (list_data_format " -- " list_data_format ", ", next_node->data_one, next_node->data_two);
			next_node = next_node->next;
		}
	}

	return;
}


list_data ListFind (struct List* dis, list_data find_data)
{
	struct ListNode* next_node = dis->head;
	for (unsigned int i = 0; i < dis->size; i++)
	{
		if (*find_data == *next_node->data_two)
			return next_node->data_one;

		next_node = next_node->next;
	}

	return NULL;
}


int ListDeleteOne (struct List* dis, list_data delete_data)
{
	struct ListNode* next_node = dis->head;
	struct ListNode* prev_node = dis->head;

	for (unsigned int i = 0; i < dis->size; i++)
	{
		if (strequ (delete_data, next_node->data_one))
		{
			prev_node->next = next_node->next;

			if (i == 0)
				dis->head = next_node->next;

			next_node->data_one = NULL;
			next_node->data_two = NULL;
			free (next_node);

			(dis->size)--;

			return 0;
		}
		else
		{
			prev_node = next_node;
			next_node = next_node->next;
		}
	}

	return -1;
}