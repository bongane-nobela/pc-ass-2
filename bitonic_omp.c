#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <stdbool.h>

// Tunable parameters
#define DEFAULT_CUTOFF 4096  // Threshold for parallel region
#define DEFAULT_THREADS omp_get_max_threads()

// Compare and swap elements according to sort direction
static inline void compare_swap(int* a, int* b, bool ascending) {
    if ((ascending && *a > *b) || (!ascending && *a < *b)) {
        int temp = *a;
        *a = *b;
        *b = temp;
    }
}

// Bitonic merge parallel implementation
void parallel_merge(int* arr, int low, int count, bool ascending) {
    if (count > 1) {
        int k = count / 2;
        
        // Parallelize the compare-swap operations
        if (count >= DEFAULT_CUTOFF) {
            #pragma omp parallel for schedule(static)
            for (int i = low; i < low + k; i++) {
                compare_swap(&arr[i], &arr[i + k], ascending);
            }
        } else {
            for (int i = low; i < low + k; i++) {
                compare_swap(&arr[i], &arr[i + k], ascending);
            }
        }
        
        // Recursively merge both halves
        parallel_merge(arr, low, k, ascending);
        parallel_merge(arr, low + k, k, ascending);
    }
}

// Bitonic sort parallel implementation
void parallel_sort(int* arr, int low, int count, bool ascending) {
    if (count > 1) {
        int k = count / 2;
        
        // Parallelize the recursive sorting
        if (count >= DEFAULT_CUTOFF) {
            #pragma omp task shared(arr) firstprivate(low, k, ascending)
            parallel_sort(arr, low, k, true);  // First half ascending
            
            #pragma omp task shared(arr) firstprivate(low, k, ascending)
            parallel_sort(arr, low + k, k, false); // Second half descending
            
            #pragma omp taskwait
        } else {
            parallel_sort(arr, low, k, true);
            parallel_sort(arr, low + k, k, false);
        }
        
        // Merge the results
        parallel_merge(arr, low, count, ascending);
    }
}

// Verify array is sorted in specified order
bool verify_sorted(const int* arr, int n, bool ascending) {
    for (int i = 1; i < n; i++) {
        if ((ascending && arr[i-1] > arr[i]) || (!ascending && arr[i-1] < arr[i])) {
            return false;
        }
    }
    return true;
}

// Check if number is power of two
bool is_power_of_two(int n) {
    return n > 0 && (n & (n - 1)) == 0;
}

// Initialize array with random values
void initialize_array(int* arr, int n) {
    #pragma omp parallel for schedule(static)
    for (int i = 0; i < n; i++) {
        arr[i] = rand();
    }
}

int main(int argc, char *argv[]) {
    // Default parameters
    int n = 1 << 20;         // Default array size (2^20)
    int num_threads = DEFAULT_THREADS;
    int cutoff = DEFAULT_CUTOFF;
    bool verbose = false;
    
    // Parse command line arguments
    if (argc > 1) n = atoi(argv[1]);
    if (argc > 2) num_threads = atoi(argv[2]);
    if (argc > 3) cutoff = atoi(argv[3]);
    if (argc > 4) verbose = atoi(argv[4]);
    
    // Validate input
    if (!is_power_of_two(n) || n < 2) {
        fprintf(stderr, "Error: Input size must be a power of two and >= 2\n");
        return EXIT_FAILURE;
    }
    
    if (num_threads < 1) num_threads = 1;
    if (cutoff < 1) cutoff = 1;
    
    // Set OpenMP parameters
    omp_set_num_threads(num_threads);
    omp_set_dynamic(0);      // Disable dynamic adjustment of threads
    
    // Allocate and initialize array
    int* arr = (int*)malloc(n * sizeof(int));
    if (!arr) {
        fprintf(stderr, "Memory allocation failed\n");
        return EXIT_FAILURE;
    }
    
    srand(time(NULL));
    initialize_array(arr, n);
    
    if (verbose) {
        printf("Initial array (first 10 elements): ");
        for (int i = 0; i < 10 && i < n; i++) printf("%d ", arr[i]);
        printf("\n");
    }
    
    // Perform parallel bitonic sort
    double start_time = omp_get_wtime();
    
    #pragma omp parallel shared(arr)
    {
        #pragma omp single
        {
            parallel_sort(arr, 0, n, true);  // Sort in ascending order
        }
    }
    
    double end_time = omp_get_wtime();
    double elapsed = end_time - start_time;
    
    // Verify and output results
    bool correct = verify_sorted(arr, n, true);
    
    // Print the results
    double serial_time = 0.0;
    if (argc > 3) serial_time = atof(argv[3]);
    double speedup = serial_time > 0.0 ? serial_time / elapsed : 0.0;
    double efficiency = speedup > 0.0 ? (speedup / num_threads) * 100.0 : 0.0;

    printf("[OpenMP] Elements: %d | Time: %.6f | PEs: %d | Speedup: %.2f | Efficiency: %.2f%% | Correct: %s\n",
           n, elapsed, num_threads, speedup, efficiency, correct ? "PASS" : "FAIL");
    
    if (verbose) {
        printf("Sorted array (first 10 elements): ");
        for (int i = 0; i < 10 && i < n; i++) printf("%d ", arr[i]);
        printf("\n");
    }
    
    free(arr);
    return EXIT_SUCCESS;
}