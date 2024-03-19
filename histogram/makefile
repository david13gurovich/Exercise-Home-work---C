CC = gcc
CFLAGS = -ansi -pedantic-errors -Wall -Wextra -g -I $(CBUFFER_H_DIR)
CBUFFER_DIR = ../../ds/cbuffer/cbuffer.c
CBUFFER_H_DIR = ../../ds/cbuffer/
UTILS_DIR = ../../ds/utils/ol62_general.h
src = $(wildcard *.c)
OBJ = $(src:.c=.o)

a.out: $(OBJ)
	$(CC) $(CFLAGS) $(CBUFFER_DIR) $< -I -o $(UTILS_DIR) -lpthread

clean:	
	rm *.o *.out

