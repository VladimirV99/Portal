PROGRAM = main
CC = gcc
CFLAGS = -Wall -Wextra
LIBS = -lGL -lGLU -lglut -lm

$(PROGRAM): main.o shared.o input.o util.o light.o portal.o
	$(CC) $(CFLAGS) $(LIBS) -o $(PROGRAM) main.o shared.o input.o util.o light.o portal.o

main.o: main.c
	$(CC) $(CFLAGS) $(LIBS) -c main.c -o main.o

shared.o: shared.c shared.h
	$(CC) $(CFLAGS) -c shared.c -o shared.o

input.o: input.c input.h
	$(CC) $(CFLAGS) -c input.c -o input.o

light.o: light.c light.h
	$(CC) $(CFLAGS) -c light.c -o light.o

util.o: util.c util.h
	$(CC) $(CFLAGS) -c util.c -o util.o

portal.o: portal.c portal.h
	$(CC) $(CFLAGS) -c portal.c -o portal.o

.PHONY: clean

clean:
	rm $(PROGRAM) *.o
