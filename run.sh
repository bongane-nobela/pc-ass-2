#!/bin/bash

TEST_SIZES=(16 32 64 256 512 1024 16384 131072 1048576 8388608)
OMP_THREADS=16
MPI_PROCS=8

for N in "${TEST_SIZES[@]}"; do
    echo "=============================================="
    echo "Testing with $N elements"
    echo "=============================================="

    #Serail v
    SERIAL_OUTPUT=$(./bitonic $N)
    SERIAL_TIME=$(echo "$SERIAL_OUTPUT" | grep -oP 'Time: \K[0-9.]+')
    echo "$SERIAL_OUTPUT"

    #OpenMP v
    export OMP_NUM_THREADS=$OMP_THREADS
    OMP_OUTPUT=$(./bitonic_omp $N $OMP_THREADS $SERIAL_TIME)
    echo "$OMP_OUTPUT"

    #MPI v
    Q=$(echo "l($N/$MPI_PROCS)/l(2)" | bc -l | awk '{printf "%d", $1}')
    P=$(echo "l($MPI_PROCS)/l(2)" | bc -l | awk '{printf "%d", $1}')
    MPI_OUTPUT=$(mpirun -np $MPI_PROCS ./bitonic_mpi $P $Q $SERIAL_TIME)
    echo "$MPI_OUTPUT"
    echo
done