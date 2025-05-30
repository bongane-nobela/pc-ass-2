#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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

    double t1 = clock() / (double)CLOCKS_PER_SEC;
    s_Bsort(arr, 0, count, 1);
    double t2 = clock() / (double)CLOCKS_PER_SEC;

    printf("Correctness (serial): %s\n", is_sorted(arr, count) ? "PASS" : "FAIL");
    printf("Sequential runtime: %.6f seconds\n", t2 - t1);

    free(arr);
    return 0;
}