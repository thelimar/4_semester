#define _CRT_SECURE_NO_WARNINGS

#include <locale.h>
#include "HashTable.h"

#define Check(error)  if (error) return error;


enum TestParams
{
	MAX_INPUT_LINE_SIZE = 30,
};

enum TestFail
{
	TEST_BUILD   = 1,
	TEST_INSERT  = 2,
	TEST_DELETE  = 3,
	TEST_FIND    = 4,
	TEST_FOREACH = 5,

};

int HashTableBuildDestroyTest ();
int HashTableInsertDeleteTest ();
int HashTableForeachTest ();
int HashTableFindTest ();


int main()
{
	setlocale(LC_ALL, "");

	int error = 0;

	for (int i = 0; i < 50; i++)
	{
		error = HashTableBuildDestroyTest();
		Check(error)

		error = HashTableInsertDeleteTest();
		Check(error)

		error = HashTableForeachTest();
		Check(error)

		error = HashTableFindTest();
		Check(error)
	}

	return 0;
}


int HashTableBuildDestroyTest ()
{
	struct HashTable Table = HashTableBuild (MurmurHash2, HASH_TABLE_SIZE);
	if (Table.DataArray == NULL) return 0;
	
	if (Table.size != HASH_TABLE_SIZE) return TEST_BUILD;

	if (Table.DataArray == NULL) return TEST_BUILD;

	if (Table.HashFunc != MurmurHash2) return TEST_BUILD; 

	HashTableDestruct (&Table);

	return 0;
}


int HashTableInsertDeleteTest ()
{
	struct HashTable Table = HashTableBuild (MurmurHash2, HASH_TABLE_SIZE);

	if (Table.DataArray == NULL) return 0;

	char* test_key =  "key";
	char* test_data = "data";
	char* test_data2 = "popa";

	if (HashTableInsert(&Table, test_key, test_data) == -1)
	{
		HashTableDestruct(&Table);
		return 0;
	}

	if (HashTableInsert(&Table, test_key, test_data) == -1)
	{
		HashTableDestruct(&Table);
		return 0;
	}

	if (Table.DataArray[MurmurHash2 (test_key, strlen(test_key)) % HASH_TABLE_SIZE].tail->data_one != test_data) return TEST_INSERT;

	HashDeleteOne (&Table, test_key, test_data2, strlen (test_key));

	HashDeleteOne(&Table, test_key, test_data, strlen(test_key));
	HashDeleteOne (&Table, test_key, test_data, strlen (test_key));

	if (HashDeleteOne(&Table, test_key, test_data, strlen(test_key)) != -1) return TEST_DELETE;

	if (Table.DataArray[MurmurHash2 (test_key, strlen(test_key)) % HASH_TABLE_SIZE].size != 0) return TEST_DELETE;

	
	if (HashTableInsert(&Table, test_key, test_data) == -1)
	{
		HashTableDestruct(&Table);
		return 0;
	}

	if (HashTableInsert(&Table, test_key, test_data) == -1)
	{
		HashTableDestruct(&Table);
		return 0;
	}

	HashDelete (&Table, MurmurHash2 (test_key, strlen (test_key)) % HASH_TABLE_SIZE);

	if (HashDelete(&Table, MurmurHash2(test_key, strlen(test_key)) % HASH_TABLE_SIZE) != -1)   return TEST_DELETE;

	if (Table.DataArray[MurmurHash2 (test_key, strlen(test_key)) % HASH_TABLE_SIZE].size != 0) return TEST_DELETE;
	

	HashTableDestruct(&Table);

	return 0;
}


int HashTableFindTest ()
{
	struct HashTable Table = HashTableBuild (MurmurHash2, HASH_TABLE_SIZE);

	if (Table.DataArray == NULL) return 0;

	char* test_key   = "key";
	char* test_data  = "data";
	char* find_str   = "key";
	char* no_find_str = "alligator";


	if (HashTableInsert(&Table, test_key, test_data) == -1)
	{
		HashTableDestruct(&Table);
		return 0;
	}

	if (HashTableInsert(&Table, test_key, test_data) == -1)
	{
		HashTableDestruct(&Table);
		return 0;
	}

	if (strcmp (HashFind (Table, find_str, strlen (find_str)), "data")) return TEST_FIND;

	if (HashFind (Table, no_find_str, strlen (no_find_str)) != NULL)    return TEST_FIND;

	HashTableDestruct (&Table);

	return 0;
}

int HashTableForeachTest ()
{
	struct HashTable Table = HashTableBuild (MurmurHash2, HASH_TABLE_SIZE);

	if (Table.DataArray == NULL) return 0;

	HashForeach (&Table, ListDump);

	char* test_key = "key";
	char* test_data = "data";

	if (HashTableInsert(&Table, test_key, test_data) == -1)
	{
		HashTableDestruct(&Table);
		return 0;
	}

	HashForeach (&Table, ListDump);

	HashTableDestruct (&Table);

	return 0;
}