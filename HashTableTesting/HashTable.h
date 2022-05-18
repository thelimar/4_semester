#pragma once

#include "OneWayLinkedList.h"

typedef char ht_data;


enum HashParams
{
	HASH_TABLE_SIZE = 12097,
	MAX_WORD_LEN = 20,
};


struct HashTable
{
	unsigned int size;
	struct List** DataArray;
	unsigned int (*HashFunc) (ht_data* str, unsigned int strlen);
};


struct HashTable* HashTableBuild(unsigned int (*HashFunc) (ht_data* str, unsigned int strlen), unsigned int size);
int HashTableDestruct(struct HashTable* dis);
ht_data* HashFind(struct HashTable* dis, ht_data* str_to_find, int strlen);
int HashForeach(struct HashTable* dis, void (*func) (ht_data* elem, void*), void* context);
int HashTableInsert(struct HashTable* dis, ht_data* key, ht_data* data);
unsigned int MurmurHash2(ht_data* key, unsigned int len);
int HashDelete(struct HashTable* dis, int hash);
int HashDeleteOne(struct HashTable* dis, ht_data* delete_key, ht_data* delete_data, int strlen);
void elem_dump(ht_data* elem, void* data);
