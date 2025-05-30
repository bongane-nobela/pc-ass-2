def b_sort(arr, low, count, D):

    k = count // 2
    if(count  > 1):

        b_sort(arr,low,k,D)
        b_sort(arr,low + k, k, 1-D)
        b_merge(arr,low,count,D)

def b_merge(arr, low, count, D):

    k = count // 2
    if(count > 1):
        
        for i in range(low, k + low):

            if( (arr[i] > arr[i+k] and D == 1) or (arr[i] < arr[i+k] and  D == 0) ):

                arr[i], arr[i+k] = arr[k+i], arr[i]

        b_merge(arr,low,k,D)
        b_merge(arr,low+k,k,D)


arr = [15,2,32,8,3,2,4,5]
print(arr)
count = len(arr)
D = 1
b_sort(arr,0,count,D)
print(arr)