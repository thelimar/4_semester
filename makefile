CC ?= gcc
OUTPUT ?= a.out
CFLAGS := ${CFLAGS} -DFAULT_INJECTION

all: compile run report

.PHONY: run report

compile: HashTest.o HashTable.o OneWayLinkedList.o
	${CC} HashTest.o HashTable.o OneWayLinkedList.o --coverage -lstdc++ ${CFLAGS}
HashTest.o: HashTableTest.c HashTable.h OneWayLinkedList.h
	${CC} -o HashTest.o HashTableTest.c -c --coverage ${CFLAGS}
HashTable.o: HashTable.h OneWayLinkedList.h HashTable.c 
	${CC} -o HashTable.o HashTable.c -c --coverage ${CFLAGS}
OneWayLinkedList.o: OneWayLinkedList.h OneWayLinkedList.c
	${CC} -o OneWayLinkedList.o OneWayLinkedList.c -c --coverage ${CFLAGS}

run:
	./${OUTPUT}
report:
	lcov -t "ht_t" -o ht.info -c -d .
	genhtml -o report ht.info

clear: clear_compile clear_report
clear_compile:
	rm -rf ${OUTPUT}
	rm -rf *.o
clear_report:
	rm -rf report
	rm -f *.gcda
	rm -f *.gcno
	rm -f ht.info
	
%.o: %.cpp %.h
	$(CC) -c $< -o $@