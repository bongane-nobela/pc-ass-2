#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define OMP_CUTOFF 4096

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
    srand(time(NULL));
    for (int i = 0; i < count; i++)
        arr[i] = rand() % 1000000;

    int num_threads = omp_get_max_threads();
    double t1 = omp_get_wtime();
    #pragma omp parallel
    {
        #pragma omp single
        {
            b_sort(arr, 0, count, 1);
        }
    }
    double t2 = omp_get_wtime();

    printf("Correctness (OpenMP): %s\n", is_sorted(arr, count) ? "PASS" : "FAIL");
    printf("Parallel runtime: %.6f seconds\n", t2 - t1);
    printf("Number of threads used (OpenMP): %d\n", num_threads);

    free(arr);
    return 0;
}