#include <stdlib.h>
#include <stdio.h>
#include <time.h>



void b_merge(int *arr, int low, int count, int D){

    int k = count / 2;
    
    if( count > 1){

        for(int i =low; i < k + low; i++){

            if (( arr[i] > arr[i + k] && D == 1) || (arr[i] < arr[i + k] && D == 0)){

                int temp = arr[i];
                arr[i] = arr[i + k];
                arr[i + k ] = temp;
            
            }

        }

        b_merge(arr, low, k, D);
        b_merge(arr, k + low, k, D);
        
    }
}
void b_sort(int *arr, int low, int count, int D){

    int k = count / 2;

    if ( count > 1){

        b_sort(arr,low,k,D);
        b_sort(arr, low + k, k, 1 -D);
        b_merge(arr, low, count, D);
    }
}



int main(){

    int count;
    
    printf("Enter the number of elements to sort: ");
    scanf("%d", &count);
    int arr[count];

    srand(time(NULL));

    printf("Before sort: \n");

    for( int i =0; i < count; i++){
        int num = rand() % 100;
        arr[i] = num;

        printf("%d ", arr[i] );
    }
    
    b_sort(arr,0,count, 1);
    printf("\nAfter sort: \n");

    for( int i =0; i < count; i++){
        printf("%d ", arr[i] );
    }
    print("\n");

    return 0;
}