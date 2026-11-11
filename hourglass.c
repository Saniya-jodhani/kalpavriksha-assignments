#include<stdio.h>
#include<limits.h>
#include<stdlib.h>

int hourglassSum(int arr_rows, int arr_columns, int** arr) {
    int maxSum = INT_MIN;
    
    for(int i=0; i< arr_rows-2;i++){
        for(int j=0; j <arr_columns-2;j++){
            int sum = arr[i][j]+ arr[i][j+1] + arr[i][j+2]+arr[i+1][j+1]+ arr[i+2][j] +arr[i+2][j+1]+arr[i+2][j+2];
            
            if(maxSum < sum){
                maxSum = sum;
            }
            
        }
    }
    return maxSum;
}

int main(){
    printf("size of array ");
    int n;
    scanf("%d",&n);
    int** arr = malloc(n*sizeof(int*));

    for(int i=0; i < n;i++){
        arr[i] = malloc(n*sizeof(int));
    }

    for(int i=0; i < n;i++){
        for(int j=0; j < n;j++){
            scanf("%d",&arr[i][j]);
        }
    }
    int result = hourglassSum(n, n, arr);
    printf("%d\n", result);

    for(int i = 0; i < n; i++) {
        free(arr[i]);
    }

    free(arr);
    return 0;
}