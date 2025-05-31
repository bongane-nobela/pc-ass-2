#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Bitonic merge
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

// Bitonic sort
void s_Bsort(int *arr, int low, int count, int D) {
    int k = count / 2;
    if (count > 1) {
        s_Bsort(arr, low, k, D);
        s_Bsort(arr, low + k, k, 1 - D);
        s_Bmerge(arr, low, count, D);
    }
}

// Check if array is sorted in ascending order
int is_sorted(int *arr, int n) {
    for (int i = 1; i < n; i++) {
        if (arr[i-1] > arr[i]) return 0;
    }
    return 1;
}

int main(int argc, char *argv[]) {
    int n = 1 << 20; // Default: 2^20 elements
    if (argc > 1) {
        n = atoi(argv[1]);
        // Ensure n is a power of two
        if (n < 2 || (n & (n - 1)) != 0) {
            printf("Input size must be a power of two and >= 2.\n");
            return 1;
        }
    }

    int *arr = malloc(n * sizeof(int));
    if (!arr) {
        printf("Memory allocation failed.\n");
        return 1;
    }

    // Seed and fill array with random integers
    srand(time(NULL));
    for (int i = 0; i < n; i++) {
        arr[i] = rand();
    }

    clock_t start = clock();
    s_Bsort(arr, 0, n, 1); // 1 for ascending order
    clock_t end = clock();

    double elapsed = (double)(end - start) / CLOCKS_PER_SEC;

    printf("[Serial] Elements: %d | Time: %.6f | PEs: 1 | Correct: %s\n",
           n, elapsed, is_sorted(arr, n) ? "PASS" : "FAIL");

    free(arr);
    return 0;
}


