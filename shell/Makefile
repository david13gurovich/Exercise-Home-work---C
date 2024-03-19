CC = gcc
CFLAGS = -ansi -pedantic-errors -Wall -Wextra

INCLUDE ?= $(LINKED_DIR)
SRCS_with_path := $(shell find . -type f -name '*.c' ! -name 'test*')
OBJS := $(SRCS_with_path:%.c=%.o) 
DS_DIR ?= ~/git/ds/
DEPFLAGS ?= -MMD -MP
LIB_DIR ?= ../lib
Archive ?= $(LIB_DIR)/libds.a
TESTS_SRCS := $(shell find $(INC_DIRS) -name test*.c) # maybe need find . -name "test*.c"
TEST_EXE := $(TESTS_SRCS:.c=.out)
DEPS := $(OBJS:.o=.d)	
DEPS_TEST := $(TESTS_SRCS:.c=.d)
LINKED_DIR ?= ../include
H_FILES := $(shell find . -type f -name '*.h')
PWD = $(shell pwd)
H_FILES_WITH_PATH := $(addprefix $(PWD)/, $(H_FILES))
TESTS_SRCS := $(shell find -name test*.c)
TEST_OBJ := $(TESTS_SRCS:.c=.o)
TEST_EXE := $(TESTS_SRCS:.c=.out)
TEST_DIR ?= ../bin

-include $(DEPS)
-include $(DEPS_TEST)

$(Archive): ./$(OBJS) 
	ar -cr $@ $(OBJS)

test_%.out: test_%.o 
	$(CC) $(CFLAGS) $< -L $(LIB_DIR) -lds -I $(LINKED_DIR) -o $(TEST_DIR)/$(notdir $@)

%.o: %.c 
	$(CC) $(CFLAGS) $(DEPFLAGS) -I $(LINKED_DIR) -c $< -o $@

test_%.o : test_%.c
	$(CC) $(CFLAGS) $(DEPFLAGS) -I $(LINKED_DIR) -c $< -o $@

fake: test_%.o 

.PHONY: all
all	: CFLAGS += -g
all: $(Archive) $(LINKED_DIR)

.PHONY: test 
test : CFLAGS += -g
test: DEPS += $(TEST_OBJ:.o =.d)
test: all $(TEST_EXE)

.PHONY: clean

clean:
	$(RM) -r  $(OBJS) $(TEST_OBJ) $(DEPS) $(DEPS_TEST) $()

.PHONY: debug
debug: 
	echo $(DEPS_TEST)