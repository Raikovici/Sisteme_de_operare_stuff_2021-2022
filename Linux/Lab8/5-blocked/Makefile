CC = gcc
CPPFLAGS = -I../utils
CFLAGS = -Wall -Wextra -Wno-unused-parameter -g
LDLIBS = -lpthread

.PHONY: all clean

all: blocked blocked2

blocked: blocked.o

blocked2: blocked2.o

blocked.o: blocked.c ../utils/utils.h

blocked2.o: blocked2.c ../utils/utils.h
	
clean:
	-rm -f *.o *~ blocked
