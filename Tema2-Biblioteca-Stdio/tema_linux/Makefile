CC = gcc
.PHONY: all clean

all: build

build: libso_stdio.so

libso_stdio.so: utils.o so_stdio.o
		$(CC) so_stdio.o utils.o -shared -o libso_stdio.so

so_stdio.o: so_stdio.c
		$(CC) -c so_stdio.c -fPIC -o so_stdio.o

utils.o: utils.c
		$(CC) -c utils.c -fPIC -o utils.o

clean:
		rm -f *.o libso_stdio.so
