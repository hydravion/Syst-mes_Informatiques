CC=gcc
CFLAGS=-Wall -g --pedantic -std=c99

all: shello

shello.o: shello.c
	$(CC) $(CFLAGS) -c -o shello.o shello.c

shello: shello.o
	$(CC) $(CFLAGS) -o shello shello.o

clean:
	rm -rf *.o shello
