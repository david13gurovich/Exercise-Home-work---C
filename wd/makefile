CC = gcc
CFLAGS = -ansi -pedantic-errors -Wall -Wextra -g -I $(H_DIR)
SCHED_DIR = ../../ds/scheduler/scheduler.c ../../ds/uid/uid.c ../../ds/task/task.c ../../ds/pqueue/pqueue.c ../../ds/sorted_list/sorted_list.c ../../ds/dll/dll.c ./test_wd.c
H_DIR = ../../ds/include/
UTILS_DIR = ../../ds/utils/ol62_general.h
src = $(wildcard *.c)
OBJ = $(src:.c=.o)

a.out: $(OBJ)
	$(CC) $(CFLAGS) $(SCHED_DIR) $< -I -o $(UTILS_DIR) -lpthread

clean:	
	rm *.o *.out

