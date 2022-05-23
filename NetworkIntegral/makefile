CC ?= gcc
OUTPUT ?= a.out
CFLAGS := ${CFLAGS} 

.PHONY: all clean

all: clean compile_server compile_client

compile_client: NetworkHeader.h Client.c ClientProcessing.c
	${CC} client.c ClientProcessing.c -o Client -lm -lpthread -std=gnu99

compile_server: NetworkHeader.h Server.c
	${CC} server.c -o Server -std=gnu99


%.o: %.c %.h
	$(CC) -c $< -o $@

clean:
	rm -f a.out
	rm -f ${OUTPUT}
	rm -f server
	rm -f client