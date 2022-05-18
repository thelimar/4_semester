HEADERS = HashTable.h OneWayLinkedList.h
SRC = HashTableTest.c HashTable.c OneWayLinkedList.c
CFLAGS=--coverage -Wall -Wextra -DFAULT_INJECTION
OBJ = $(patsubst %.c,%.o,$(SRC))
VPATH=../

PHONY: compile valgrind coverage clean

compile: code
	./code

valgrind: code
	valgrind ./code

code: $(OBJ)
	gcc -o code --coverage $^

coverage: compile
	lcov -t "ht_t" -o ht.info -c -d .
	genhtml -o report ht.info
	gcov $(patsubst %.gcda,%.c,$(SRC))

clean:
	rm *.o *.gcno *.gcda *gcov code

include $(wildcard *.d)