CC = gcc
CFLAGS = -ansi -pedantic-errors -Wall -Wextra -g 
src = $(wildcard *.c)
OBJ = $(src:.c=.o)

a.out: $(OBJ)
	$(CC) $(CFLAGS) $< -I -o -lpthread

clean:	
	rm *.o *.out
