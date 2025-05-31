CC = gcc
MPICC = mpicc
CFLAGS = -O3 -Wall

all: bitonic bitonic_omp bitonic_mpi

bitonic: bitonic.c
	$(CC) $(CFLAGS) -o $@ $^

bitonic_omp: bitonic_omp.c
	$(CC) $(CFLAGS) -fopenmp -o $@ $^

bitonic_mpi: bitonic_mpi.c
	$(MPICC) $(CFLAGS) -o $@ $^

clean:
	rm -f bitonic bitonic_omp bitonic_mpi