#define _CRT_0SECURE_NO_WARNINGS

#include <locale.h>
#include "OneWayLinkedList.h"

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

typedef char ht_data;


enum HashParams
{
	HASH_TABLE_SIZE = 12097,
	MAX_WORD_LEN = 20,
};


struct HashTable
{
	unsigned int size;
	struct List* DataArray;
	unsigned int (*HashFunc) (ht_data* str, unsigned int strlen);
};


struct HashTable HashTableBuild (unsigned int (*HashFunc) (ht_data* str, unsigned int strlen), unsigned int size);
int HashTableDestruct (struct HashTable* dis);
ht_data* HashFind (struct HashTable dis, ht_data* str_to_find, int strlen);
int HashForeach (struct HashTable* dis, void(*func)(struct List));
int HashTableInsert (struct HashTable* dis, ht_data* key, ht_data* data);
unsigned int MurmurHash2 (ht_data* key, unsigned int len);
int HashDelete(struct HashTable* dis, int hash);
int HashDeleteOne(struct HashTable* dis, ht_data* delete_key, ht_data* delete_data, int strlen); 
static void* FI_calloc(size_t size, size_t size_of);



struct HashTable HashTableBuild (unsigned int (*HashFunc) (ht_data* str, unsigned int strlen), unsigned int size)
{
	struct HashTable dis_hashtable = { size, 0, HashFunc};

	dis_hashtable.DataArray = (struct List*) FI_calloc (size, sizeof (struct List));


	if (dis_hashtable.DataArray)
	{
		for (int i = 0; i < size; i++)
			dis_hashtable.DataArray[i] = ListBuild ();
	}


	return dis_hashtable;
}


int HashTableDestruct (struct HashTable* dis)
{
	if (!dis) return 0;

	for (unsigned int i = 0; i < dis->size; i++)
		if (dis->DataArray[i].size != 0) ListDestruct (&dis->DataArray[i]);

	free (dis->DataArray);
	return 0;
}


int HashTableInsert (struct HashTable* dis, ht_data* key, ht_data* data)
{
	if (ListPush (&dis->DataArray[dis->HashFunc (key, strlen ((char*) key)) % dis->size], data, key) == -1)
		return -1;

	return 0;
}


ht_data* HashFind (struct HashTable dis, ht_data* str_to_find, int strlen)
{
	unsigned int hash = dis.HashFunc (str_to_find, strlen) % dis.size;
	if (dis.DataArray[hash].size == 0)
		return NULL;
	else
		return ListFind (dis.DataArray[hash], str_to_find);
}

int HashDelete (struct HashTable* dis, int hash)
{
	if (dis->DataArray[hash].size == 0)
		return -1;
	else
		ListDestruct (&dis->DataArray[hash]);

	return 0;
}


int HashDeleteOne (struct HashTable* dis, ht_data* delete_key, ht_data* delete_data, int strlen)
{
	unsigned int hash = dis->HashFunc (delete_key, strlen) % dis->size;

	if (dis->DataArray[hash].size == 0)
		return -1;
	else
		return ListDeleteOne (&dis->DataArray[hash], delete_data);
}


unsigned int MurmurHash2 (ht_data* key, unsigned int len)
{
	const unsigned int m = 0x5bd1e995;
	const unsigned int seed = 0;
	const int r = 24;

	unsigned int h = seed ^ len;

	const unsigned char* data = (const unsigned char*)key;
	unsigned int k = 0;

	while (len >= 4)
	{
		k = data[0];
		k |= data[1] << 8;
		k |= data[2] << 16;
		k |= data[3] << 24;

		k *= m;
		k ^= k >> r;
		k *= m;

		h *= m;
		h ^= k;

		data += 4;
		len -= 4;
	}

	switch (len)
	{
	case 3:
		h ^= data[2] << 16;
	case 2:
		h ^= data[1] << 8;
	case 1:
		h ^= data[0];
		h *= m;
	};

	h ^= h >> 13;
	h *= m;
	h ^= h >> 15;

	return h;
}

int HashForeach (struct HashTable* dis, void(*func)(struct List))
{
	for (int i = 0; i < dis->size; i++)
	{
		func (dis->DataArray[i]);
	}

	return 0;
}