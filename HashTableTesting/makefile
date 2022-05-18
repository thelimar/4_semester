HEADERS = HashTable.h OneWayLinkedList.h
SRC = HashTableTest.c HashTable.c OneWayLinkedList.c
CFLAGS=--coverage -Wall -Wextra -DFAULT_INJECTION  -MMD -MP
OBJ = HashTableTest.o HashTable.o OneWayLinkedList.o
VPATH=../

PHONY: compile valgrind coverage clean

compile: code
	./code

valgrind: code
	valgrind ./code

code: $(OBJ)
	gcc -o code --coverage $(OBJ)

$(OBJ): $(SRC)
	gcc $(CFLAGS) -c $(SRC)


coverage: compile
	lcov -t "ht_t" -o ht.info -c -d .
	genhtml -o report ht.info
	gcov $(patsubst %.gcda,%.c,$(SRC))

clean:
	rm *.o *.gcno *.gcda *gcov code

-include *.d