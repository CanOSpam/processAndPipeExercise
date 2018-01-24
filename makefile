CC=gcc
CFLAGS=-I.

all: PipesAndForks.c
	$(CC) -o pipes PipesAndForks.c
