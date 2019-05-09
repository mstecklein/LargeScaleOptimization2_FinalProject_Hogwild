CC=gcc
CFLAGS=-I. -pthread -lm -D_GNU_SOURCE
EXE=run
DEPS = $(wildcard *.h)
SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(EXE): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)


.PHONY: clean
clean:
	    rm $(EXE) *.o
