#include <stdio.h>
#include <stdlib.h>
#include <time.h>


void serial_merge(int *arr, int low, int count, int D) {
    int k = count / 2;
    if (count > 1) {
        for (int i = low; i < k + low; i++) {
            if ((arr[i] > arr[i + k] && D == 1) || (arr[i] < arr[i + k] && D == 0)) {

                int temp = arr[i];
                arr[i] = arr[i + k];

                arr[i + k] = temp;
            }
        }
        serial_merge(arr, low, k, D);

        serial_merge(arr, k + low, k, D);
    }
}

void serial_sort(int *arr, int low, int count, int D) {



    int k = count / 2;
    if (count > 1) {

        
        serial_sort(arr, low, k, D);
        serial_sort(arr, low + k, k, 1 - D);
        serial_merge(arr, low, count, D);
    }
}

int is_sorted(int *arr, int n) {
    for (int i = 1; i < n; i++) {

        if (arr[i-1] > arr[i]) return 0;
    }
    return 1;
}

int main(int argc, char *argv[]) {
    int n = 1 << 20; 
    if (argc > 1) {

        n = atoi(argv[1]);
        
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




    srand(time(NULL));

    for (int i = 0; i < n; i++) {
        arr[i] = rand();


    }



    clock_t start = clock();
    serial_sort(arr, 0, n, 1); 

    clock_t end = clock();

    double elapsed = (double)(end - start) / CLOCKS_PER_SEC;

    printf("[Serial] Elements: %d | Time: %.6f | PEs: 1 | Correct: %s\n",


           n, elapsed, is_sorted(arr, n) ? "PASS" : "FAIL");

    free(arr);




    return 0;
}


