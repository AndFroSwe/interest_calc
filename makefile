# Makefile for interest calculator
CC = gcc
FLAGS =
LIBS = -lncurses -lform

# Phonys
all: interest.out

clean:
	rm -rf *.out *.o

# Source compilation
interest.out: src/interest.c src/interest.h
	$(CC) $(FLAGS) -o $@ $< $(LIBS)

.PHONY: all clean
