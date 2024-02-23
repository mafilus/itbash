CC=gcc
TARGET=itbash
DEBUG=-g
FLAGS=-Wall -Werror -Wextra -pedantic
CFLAGS=-s

all: main.o 
	$(CC) $(LIB) *.o -o $(TARGET) $(CFLAGS)

main.o: main.c 
	$(CC) -c main.c $(DEBUG) $(FLAGS)

clean:
	rm *.o *~
