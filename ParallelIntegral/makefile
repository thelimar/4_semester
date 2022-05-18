СС = gcc
HEADERS = Integral.h
SRC = Integral.c ParallelComputing.c
CFLAGS = -Wall
OBJ = $(patsubst %.c,%.o,$(SRC))
VPATH=../

PHONY: compile


compile: code


code: $(OBJ)
	$(CC) -o code $^ -lm  -lpthread


clean:
	rm *.o code

include $(wildcard *.d)