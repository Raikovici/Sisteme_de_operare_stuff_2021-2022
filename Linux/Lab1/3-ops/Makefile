CC = gcc
CFLAGS = -Wall
LDLIBS = -L.

all: ops

ops: add.o mul.o ops.o
	$(CC) $(CFLAGS) $^ -lm -L /usr/lib/x86_64-linux-gnu -o $@

ops.o: ops.c
	$(CC) $(CFLAGS) -c $^ -D HAVE_MATH -o $@
add.o: add.c
	$(CC) $(CFLAGS) -c $^ -o $@
mul.o: mul.c
	$(CC) $(CFLAGS) -c $^ -o $@
clean:
	rm -rf *.o ops

