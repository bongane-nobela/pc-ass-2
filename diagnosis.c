#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <omp.h>

// Serial sort prototypes
void s_Bsort(int *arr, int low, int count, int D);
// OMP sort prototypes
void b_sort(int *arr, int low, int count, int D);

int is_sorted(int *arr, int n) {
    for (int i = 1; i < n; i++)
        if (arr[i-1] > arr[i]) return 0;
    return 1;
}

int is_power_of_two(int n) {
    return n > 0 && (n & (n - 1)) == 0;
}

int main() {
    int count;
    do {
        printf("Enter the number of elements to sort (power of 2): ");
        fflush(stdout);
        scanf("%d", &count);
        if (!is_power_of_two(count)) {
            printf("Error: Number must be a power of 2.\n");
        }
    } while (!is_power_of_two(count));

    int *arr = malloc(count * sizeof(int));
    int *arr_serial = malloc(count * sizeof(int));
    int *arr_parallel = malloc(count * sizeof(int));

    srand(time(NULL));
    for (int i = 0; i < count; i++) {
        int num = rand() % 1000000;
        arr[i] = num;
        arr_serial[i] = num;
        arr_parallel[i] = num;
    }

    printf("\nInput size: %d\n", count);

    // Serial
    double t1 = omp_get_wtime();
    s_Bsort(arr_serial, 0, count, 1);
    double t2 = omp_get_wtime();
    printf("/////////////////////// SERIAL //////////////////////\n");
    printf("Correctness (serial): %s\n", is_sorted(arr_serial, count) ? "PASS" : "FAIL");
    double serial_time = t2 - t1;
    printf("Sequential runtime: %.6f seconds\n", serial_time);

    // OpenMP
    int num_threads = omp_get_max_threads();
    double t3 = omp_get_wtime();
    #pragma omp parallel
    {
        #pragma omp single
        {
            b_sort(arr_parallel, 0, count, 1);
        }
    }
    double t4 = omp_get_wtime();
    printf("/////////////////////// OpenMP //////////////////////\n");
    printf("Correctness (OpenMP): %s\n", is_sorted(arr_parallel, count) ? "PASS" : "FAIL");
    double parallel_time = t4 - t3;
    printf("Parallel runtime: %.6f seconds\n", parallel_time);

    if (parallel_time > 0)
        printf("Achieved speedup: %.2f\n", serial_time / parallel_time);
    else
        printf("Achieved speedup: N/A (parallel time too small)\n");

    printf("Number of threads used (OpenMP): %d\n", num_threads);

    printf("\nTo test MPI, run: mpirun -np <procs> ./mpi_sort\n");

    free(arr);
    free(arr_serial);
    free(arr_parallel);

    return 0;
}