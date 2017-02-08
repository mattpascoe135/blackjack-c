CC = gcc
CFLAGS = -Wall -pthread -pedantic -std=gnu99
DEBUG = -g
DEPS = deck.c
TARGETS = server client

.DEFAULT: all

.PHONY: all debug clean


all: $(TARGETS)

debug: CFLAGS += $(DEBUG)
debug: clean $(TARGETS)

deck.o: deck.c deck.h
	$(CC) $(CFLAGS) -c deck.c -o deck.o

server: server.c deck.o
	$(CC) $(CFLAGS) deck.o server.c -o server 

client: client.c deck.o
	$(CC) $(CFLAGS) deck.o client.c -o client

clean:$ 
	rm -f $(TARGETS) *.o
