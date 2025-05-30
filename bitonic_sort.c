#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <omp.h>

#define OMP_CUTOFF 4096

////////////////////////////////////////////////////// SERIAL ////////////////////////////////////////////////////////////

void s_Bmerge(int *arr, int low, int count, int D) {
    int k = count / 2;
    if (count > 1) {
        for (int i = low; i < k + low; i++) {
            if ((arr[i] > arr[i + k] && D == 1) || (arr[i] < arr[i + k] && D == 0)) {
                int temp = arr[i];
                arr[i] = arr[i + k];
                arr[i + k] = temp;
            }
        }
        s_Bmerge(arr, low, k, D);
        s_Bmerge(arr, k + low, k, D);
    }
}

void s_Bsort(int *arr, int low, int count, int D) {
    int k = count / 2;
    if (count > 1) {
        s_Bsort(arr, low, k, D);
        s_Bsort(arr, low + k, k, 1 - D);
        s_Bmerge(arr, low, count, D);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////// OpenMP ////////////////////////////////////////////////////////////////

void b_merge(int* arr, int low, int count, int D) {
    int k = count / 2;
    if (count > 1) {
        if (count >= OMP_CUTOFF) {
            #pragma omp parallel for
            for (int i = low; i < low + k; i++) {
                if ((arr[i] > arr[i + k] && D == 1) || (arr[i] < arr[i + k] && D == 0)) {
                    int temp = arr[i];
                    arr[i] = arr[i + k];
                    arr[i + k] = temp;
                }
            }
        } else {
            for (int i = low; i < low + k; i++) {
                if ((arr[i] > arr[i + k] && D == 1) || (arr[i] < arr[i + k] && D == 0)) {
                    int temp = arr[i];
                    arr[i] = arr[i + k];
                    arr[i + k] = temp;
                }
            }
        }
        b_merge(arr, low, k, D);
        b_merge(arr, low + k, k, D);
    }
}

void b_sort(int* arr, int low, int count, int D) {
    int k = count / 2;
    if (count > 1) {
        if (count >= OMP_CUTOFF) {
            #pragma omp task shared(arr)
            b_sort(arr, low, k, D);
            #pragma omp task shared(arr)
            b_sort(arr, low + k, k, 1 - D);
            #pragma omp taskwait
        } else {
            b_sort(arr, low, k, D);
            b_sort(arr, low + k, k, 1 - D);
        }
        b_merge(arr, low, count, D);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int is_sorted(int *arr, int n) {
    for (int i = 1; i < n; i++) {
        if (arr[i-1] > arr[i]) return 0;
    }
    return 1;
}

// Helper function to check if n is a power of 2
int is_power_of_two(int n) {
    return n > 0 && (n & (n - 1)) == 0;
}

int main() {
    int count;
    do {
        printf("Enter the number of elements to sort (power of 2): ");
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
        int num = rand() % 100;
        arr[i] = num;
        arr_serial[i] = num;
        arr_parallel[i] = num;
    }
    printf("\n");

    int num_threads = omp_get_max_threads();
    printf("Input size: %d\n", count);
    printf("Max parallel PEs: %d\n", num_threads);

    double t1 = omp_get_wtime();
    s_Bsort(arr_serial, 0, count, 1);
    double t2 = omp_get_wtime();
    printf("/////////////////////// SERIAL //////////////////////\n");
    printf("Correctness (serial): %s\n", is_sorted(arr_serial, count) ? "PASS" : "FAIL");
    double serial_time = t2 - t1;
    printf("Sequential runtime: %.6f seconds\n", serial_time);

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

    free(arr);
    free(arr_serial);
    free(arr_parallel);

    return 0;
}