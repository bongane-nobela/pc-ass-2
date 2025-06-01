#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>
#include <stdbool.h>

#define ASCENDING 1
#define DESCENDING 0
#define MASTER 0


int is_power_of_two(int n) {


    return n > 0 && (n & (n - 1)) == 0;
}


int ascendingOrder(const void *a, const void *b) {


    return (*(int*)a) - (*(int*)b);
}

void merge(int **a, int *b, size_t N, int dir) {

    int *result = malloc(N * sizeof(int));

    int i = 0, j = 0, k = 0;
    if (dir == ASCENDING) {
       
        while (i < N && j < N && k < N) {

            if ((*a)[i] < b[j]) result[k++] = (*a)[i++];

            else result[k++] = b[j++];


        }
        while (k < N && i < N) result[k++] = (*a)[i++];
        while (k < N && j < N) result[k++] = b[j++];


    } else {
       
        i = N - 1; j = N - 1; k = N - 1;

        while (i >= 0 && j >= 0 && k >= 0) {

            if ((*a)[i] > b[j]) result[k--] = (*a)[i--];
            else result[k--] = b[j--];
        }
        while (k >= 0 && i >= 0) result[k--] = (*a)[i--];
        while (k >= 0 && j >= 0) result[k--] = b[j--];
    }
    free(*a);
    *a = result;
}


void compare(int **local_array, size_t N, int partner, int dir) {

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int *recv_buf = malloc(N * sizeof(int));
    MPI_Sendrecv(*local_array, N, MPI_INT, partner, 0,

                 recv_buf, N, MPI_INT, partner, 0,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    merge(local_array, recv_buf, N, dir);

    free(recv_buf);
}


bool verify_sorted(const int* arr, int n) {
    for (int i = 1; i < n; i++) {
        if (arr[i - 1] > arr[i]) return false;
    }
    return true;
}

int main(int argc, char **argv) {
    int rank, size, p, q, N;

    int *array;
    double startTime = 0, endTime = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    MPI_Comm_size(MPI_COMM_WORLD, &size);





    if (argc < 3) {

        if (rank == 0) {
            printf("Usage: %s p q\n", argv[0]);

            printf("  p: log2(number of MPI processes)\n");
            printf("  q: log2(number of elements per process)\n");
        }
        MPI_Finalize();
        return 1;
    }

    p = atoi(argv[1]);


    q = atoi(argv[2]);

    if (!is_power_of_two(size) || size != (1 << p)) {


        if (rank == 0)
            printf("Number of processes must be 2^p and a power of two.\n");
        MPI_Abort(MPI_COMM_WORLD, 2);

        return 2;
    }

    N = 1 << q;



    array = malloc(N * sizeof(int));

    if (!array) {

        printf("Rank %d: Memory allocation failed.\n", rank);
        MPI_Abort(MPI_COMM_WORLD, 3);

        return 3;
    }

    srand(time(NULL) * (rank + 1));
    for (int i = 0; i < N; i++)
        array[i] = rand() % (N * size);

   
    qsort(array, N, sizeof(int), ascendingOrder);
    
    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == MASTER)
        startTime = MPI_Wtime();


    for (int i = 0; i < p; i++) {

        for (int j = i; j >= 0; j--) {

            int partner = rank ^ (1 << j);

            int keep_small = (((rank >> (i + 1)) & 1) == ((rank >> j) & 1));


            if (keep_small)

                compare(&array, N, partner, ASCENDING);

            else

                compare(&array, N, partner, DESCENDING);
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == MASTER)
        endTime = MPI_Wtime();



    int *all_data = NULL;

    if (rank == MASTER)
        all_data = malloc(N * size * sizeof(int));

    MPI_Gather(array, N, MPI_INT, all_data, N, MPI_INT, MASTER, MPI_COMM_WORLD);

    double serial_time = 0.0;


    if (argc > 3) serial_time = atof(argv[3]);




    double mpi_time = endTime - startTime;
    double speedup = (serial_time > 0.0) ? serial_time / mpi_time : 0.0;

    double efficiency = (speedup > 0.0) ? (speedup / size) * 100.0 : 0.0;

    if (rank == MASTER) {
        printf("[MPI] Elements: %d | Time: %.6f | PEs: %d | Speedup: %.2f | Efficiency: %.2f%% | Correctly sorted: %s\n",

               N * size, mpi_time, size, speedup, efficiency,

               verify_sorted(all_data, N * size) ? "PASS" : "FAIL");

        free(all_data);
    }

    free(array);
    MPI_Finalize();
    return 0;
}
