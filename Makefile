CC=gcc
CFLAGS=-g -O3 -mfpmath=sse -msse -mmmx -Wall -ffast-math
LIBS=-lSDL -lGL -lGLU

ALL_OBJS=tile.o swarm.o vector.o

all: swarm

clean:
	rm -f swarm *.o

depend:
	$(CC) -MM *.c > .depend

swarm: $(ALL_OBJS)
	$(CC) -o swarm $(LIBS) $(ALL_OBJS)

-include .depend
