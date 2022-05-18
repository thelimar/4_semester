#pragma once

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

//#define DEBUG

#ifndef DEBUG
#define DBG if(0)
#else
#define DBG
#endif


#define list_data_format "%s"


enum ListErrors
{
	SIZE_TOO_BIG = 1,
	SIZE_TOO_SMALL = 2,
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

struct List* ListBuild ();
struct ListNode* ListNodeBuild (list_data node_data_one, list_data node_data_two);
void ListNodeDestruct (struct ListNode* dis);
void ListDestruct (struct List* dis);
int ListPush (struct List* dis, list_data node_data_one, list_data node_data_two);
list_data ListFind (struct List* dis, list_data find_data);
void ListDump (struct List* dis);
int ListDeleteOne (struct List* dis, list_data delete_data);
