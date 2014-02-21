CC=gcc
CFLAGS=-g -O2 -mfpmath=sse -msse -mmmx -Wall -ffast-math
LIBS=-lSDL -lGL -lGLU -lm

ALL_OBJS=tile.o swarm.o vector.o

all: swarm

clean:
	rm -f swarm *.o

depend:
	$(CC) -MM *.c > .depend

swarm: $(ALL_OBJS)
	$(CC) -o swarm $(ALL_OBJS) $(LIBS)

-include .depend
