#!/bin/bash

make clean
make

echo "===== SERIAL ====="
./serial_sort

echo
echo "===== OPENMP ====="
./omp_sort

echo
echo "===== DIAGNOSIS (Serial + OpenMP) ====="
./diagnosis

echo
echo "===== MPI (run with 4 processes as example) ====="
mpirun -np 4 ./mpi_sort

make clean