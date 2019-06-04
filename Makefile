CC = gcc
CFLAGS = -Wall -pedantic
LDFLAGS = 
DEBUG = -g

all: minls minget

minls: minls.o minFuncs.o
	$(CC) $(DEBUG) $(LDFLAGS) -o $@ $^

minget: minget.o minFuncs.o
	$(CC) $(DEBUG) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

clean:
	rm -rf *.o core
