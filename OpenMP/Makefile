CC = gcc
CFLAGS = -Wall -fopenmp
LDFLAGS = -lm

all: docs-omp

docs-omp: docs-omp.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

clean:
	rm -f docs-omp
