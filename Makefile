basic : basic.o
	gcc -g `pkg-config fuse --libs` -o basic basic.o

basic.o : basic.c
	gcc -g -Wall `pkg-config fuse --cflags` -c basic.c

clean:
	rm -f basic *.o
