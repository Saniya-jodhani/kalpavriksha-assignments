#include<stdio.h>

void swap(int *a , int* b){
    int temp = *a;
    *a = *b;
    *b = temp;
}
void printMatrix(int n , int matrix[n][n]){
    for(int i=0; i < n;i++){
        for(int j=0; j < n;j++){
            printf("%d ",matrix[i][j]);
        }
        printf("\n");
    }
}

int main(){
    int n;
    printf("Enter size of the matrix: ");
    scanf("%d", &n);
    int matrix[n][n]; 
    printf("Enter elements of the matrix: \n");
    for(int i=0; i < n;i++){
        for(int j=0; j < n;j++){
            scanf("%d",&matrix[i][j]);
        }
    }

    printf("Original :\n");
    printMatrix(n,matrix);


    for(int i=0; i < n;i++){
        for(int j = i;j < n;j++){
            swap(&matrix[i][j], &matrix[j][i]);
        }
    }
    for(int i=0; i < n;i++){
        for(int j=0; j < n/2;j++){
            swap(&matrix[i][j], &matrix[i][n-j-1]);
        }
    }
   
    printf("\nRotated:\n");
    printMatrix(n,matrix);

    
   for(int i=0; i < n;i++){
    for(int j =0; j < n;j++){
        int sum = 0;
        int count =0;

        for(int r = i-1;r<=i+1; r++){
            for(int c = j-1; c<=j+1;c++){
                if(r>=0 && r<n && c>=0 && c< n){
                    int* neighbour = *(matrix+r)+c;
                    int value = *neighbour;

                    if(value>= 256){
                        value = value/256-1;
                    }
                    sum+=value;
                    count++;

                }
            }
        }

        int average = sum/count;

        int *cell = *(matrix+i)+j;
        int original = *cell;
        
        *cell = (original+1)*256+average;
    }
   }
   for(int i =0; i < n;i++){
    for(int j =0; j <n;j++){
        int *cell = *(matrix+i)+j;
        *cell = *cell%256;
    }
   }


    
    printf("\nFinal Output:\n");
    for(int i=0; i < n;i++){
        for(int j=0; j < n;j++){
            printf("%d ",matrix[i][j]);
        }
        printf("\n");
    }
    
}