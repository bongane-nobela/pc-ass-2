#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

#define UP 1
#define DOWN 0

// Helper function to check if n is a power of 2
int is_power_of_two(int n) {
    return n > 0 && (n & (n - 1)) == 0;
}

// Local compare and swap
void compare_swap(int *local, int partner, int dir) {
    int temp;
    if ((dir == UP && *local > partner) || (dir == DOWN && *local < partner)) {
        temp = *local;
        *local = partner;
        partner = temp;
    }
}

// Local sort (simple, since each process gets a chunk)
void local_sort(int *arr, int n) {
    // You can use qsort or implement your own
    for (int i = 0; i < n-1; i++)
        for (int j = 0; j < n-i-1; j++)
            if (arr[j] > arr[j+1]) {
                int t = arr[j];
                arr[j] = arr[j+1];
                arr[j+1] = t;
            }
}

// Merge two sorted arrays in bitonic fashion
void bitonic_merge(int *local, int *recv_buf, int n, int dir) {
    int *temp = malloc(n * sizeof(int));
    int i = 0, j = 0, k = 0;
    if (dir == UP) {
        while (i < n && j < n) {
            if (local[i] < recv_buf[j])
                temp[k++] = local[i++];
            else
                temp[k++] = recv_buf[j++];
        }
    } else {
        while (i < n && j < n) {
            if (local[i] > recv_buf[j])
                temp[k++] = local[i++];
            else
                temp[k++] = recv_buf[j++];
        }
    }
    while (i < n) temp[k++] = local[i++];
    while (j < n) temp[k++] = recv_buf[j++];
    for (i = 0; i < n; i++) local[i] = temp[i];
    free(temp);
}

int is_sorted(int *arr, int n) {
    for (int i = 1; i < n; i++)
        if (arr[i-1] > arr[i]) return 0;
    return 1;
}

int main(int argc, char **argv) {
    int rank, size, n, *data = NULL, *local, local_n;
    double t1, t2;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        printf("Enter the number of elements to sort (power of 2): ");
        fflush(stdout);
        scanf("%d", &n);
        if (!is_power_of_two(n) || !is_power_of_two(size) || n % size != 0) {
            printf("Number of elements and number of processes must be powers of 2 and n %% size == 0.\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        data = malloc(n * sizeof(int));
        srand(time(NULL));
        for (int i = 0; i < n; i++)
            data[i] = rand() % 100;
    }

    // Broadcast n to all processes
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    local_n = n / size;
    local = malloc(local_n * sizeof(int));

    // Scatter data to all processes
    MPI_Scatter(data, local_n, MPI_INT, local, local_n, MPI_INT, 0, MPI_COMM_WORLD);

    // Local sort
    local_sort(local, local_n);

    t1 = MPI_Wtime();

    // Main bitonic sort
    for (int k = 2; k <= size; k *= 2) {
        for (int j = k / 2; j > 0; j /= 2) {
            int partner = rank ^ j;
            int dir = ((rank & k) == 0) ? UP : DOWN;

            int *recv_buf = malloc(local_n * sizeof(int));
            MPI_Sendrecv(local, local_n, MPI_INT, partner, 0,
                         recv_buf, local_n, MPI_INT, partner, 0,
                         MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            // Merge
            if ((rank & k) == 0) {
                if (rank < partner)
                    bitonic_merge(local, recv_buf, local_n, UP);
                else
                    bitonic_merge(local, recv_buf, local_n, DOWN);
            } else {
                if (rank > partner)
                    bitonic_merge(local, recv_buf, local_n, UP);
                else
                    bitonic_merge(local, recv_buf, local_n, DOWN);
            }
            free(recv_buf);
        }
    }

    t2 = MPI_Wtime();

    // Gather sorted data to root
    MPI_Gather(local, local_n, MPI_INT, data, local_n, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("/////////////////////// MPI //////////////////////\n");
        printf("Correctness (MPI): %s\n", is_sorted(data, n) ? "PASS" : "FAIL");
        printf("Parallel runtime: %.6f seconds\n", t2 - t1);
        // Optionally print sorted array for small n
        // for (int i = 0; i < n; i++) printf("%d ", data[i]); printf("\n");
        free(data);
    }
    free(local);

    MPI_Finalize();
    return 0;
}