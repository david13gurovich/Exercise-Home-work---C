CC=gcc
CFLAGS = -ansi -pedantic-errors -Wall -Wextra 
SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)

a.out: $(OBJ)
	$(CC) $(CFLAGS) $< -I -o -lpthread -fopenmp

clean:
	rm *.o *.out