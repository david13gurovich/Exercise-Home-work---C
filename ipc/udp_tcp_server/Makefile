CC=g++
CPPFLAGS=-pedantic -Wall -Wextra -pthread
SRCS = main.cpp console.cpp tcp_server.c udp_server.c ../../../cpp/logger/logger.cpp
OBJS = $(SRCS:.cpp=.o)

.PHONY: gdp
gdp: CPPFLAGS += -std=c++98 -g
gdp: all

.PHONY: all
all: a.out

a.out: main.o logger.o console.o udp_server.o tcp_server.o
	$(CC) ${CPPFLAGS} $^ -o $@

main.o: main.c console.h tcp_server.h udp_server.h ../../../cpp/logger/logger.hpp
	$(CC) -c $(CPPFLAGS) -I ../../../cpp/logger/ main.c -o $@

console.o: console.c consol.h ../../../cpp/logger/logger.hpp
	$(CC) -c $(CPPFLAGS) -I ../../../cpp/logger/ console.c -o $@

udp_server.o: udp_server.c udp_server.h ../../../cpp/logger/logger.hpp
	$(CC) -c $(CPPFLAGS) -I ../../../cpp/logger/ udp_server.c -o $@

tcp_server.o: tcp_server.c tcp_server.h ../../../cpp/logger/logger.hpp
	$(CC) -c $(CPPFLAGS) -I ../../../cpp/logger/ tcp_server.c -o $@

logger.o: ../../../cpp/logger/logger.cpp ../../../cpp/logger/logger.hpp
	$(CC) -c $(CPPFLAGS) -I ../../../cpp/logger/ ../../../cpp/logger/logger.cpp -o $@

.PHONY: gc11
gc11: CC += -std=c++11 -Werror
gc11: all

.PHONY: gd11
gd11: CC += -std=c++11 -Werror -g
gd11: all

.PHONY: gdpp
gdpp: CPPFLAGS += -std=c++98 -Werror -g
gdpp: all

.PHONY: gcp
gcp: CPPFLAGS += -std=c++98 -Werror -DNDEBUG -O3
gcp: all

.PHONY: debug
debug:
	echo $(TARGET_LIB)

.PHONY: clean
clean:
	rm -f *.o
