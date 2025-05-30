CC=gcc
MPICC=mpicc
CFLAGS=-O2
OMPFLAGS=-fopenmp

all: serial_sort omp_sort mpi_sort diagnosis

serial_sort: serial_sort.c
	$(CC) $(CFLAGS) -o serial_sort serial_sort.c

omp_sort: omp_sort.c
	$(CC) $(CFLAGS) $(OMPFLAGS) -o omp_sort omp_sort.c

mpi_sort: mpi_sort.c
	$(MPICC) $(CFLAGS) -o mpi_sort mpi_sort.c

diagnosis: diagnosis.c serial_sort.c omp_sort.c
	$(CC) $(CFLAGS) $(OMPFLAGS) -o diagnosis diagnosis.c serial_sort.c omp_sort.c

clean:
	rm -f serial_sort omp_sort mpi_sort diagnosis *.o

.PHONY: all clean