#!/bin/bash

# Serial version
echo "Running serial version..."
SERIAL_OUTPUT=$(./bitonic ${1:-1048576})
SERIAL_TIME=$(echo "$SERIAL_OUTPUT" | grep -oP 'Time: \K[0-9.]+')
echo "$SERIAL_OUTPUT"

# OpenMP version
echo -e "\nRunning OpenMP version..."
export OMP_NUM_THREADS=${2:-4}
OMP_OUTPUT=$(./bitonic_omp ${1:-1048576} $OMP_NUM_THREADS $SERIAL_TIME)
echo "$OMP_OUTPUT"

# MPI version
echo -e "\nRunning MPI version..."
NP=${3:-4}
N=${1:-1048576}
Q=$(echo "l($N/$NP)/l(2)" | bc -l | awk '{printf "%d", $1}')
P=$(echo "l($NP)/l(2)" | bc -l | awk '{printf "%d", $1}')
MPI_OUTPUT=$(mpirun -np $NP ./bitonic_mpi $P $Q $SERIAL_TIME)
echo "$MPI_OUTPUT"