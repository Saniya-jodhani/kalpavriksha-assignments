#include<stdio.h>
int main(){
    int h , w;
    scanf("%d %d", &h, &w);

    int matrix[h][w];
    for(int i=0; i < h;i++){
        for(int j=0; j < w;j++){
            scanf("%d",&matrix[i][j]);
        }
    }

    int area =0;

    for(int i=0; i < h;i++){
        for(int j=0; j < w;j++){
            int curr = matrix[i][j];
            if(curr==0) continue;

            area+=2;

            if(i==0) area+=curr;
            else area +=(curr>matrix[i-1][j])? curr-matrix[i-1][j]: 0;

            if(i==h-1) area+=curr;
            else area +=(curr>matrix[i+1][j])? curr-matrix[i+1][j]:0;

            if(j==0) area+=curr;
            else area += (curr>matrix[i][j-1])? curr-matrix[i][j-1]:0;

            if(j==w-1) area+=curr;
            else area += (curr>matrix[i][j+1])? curr-matrix[i][j+1]:0;
        }
    }
    printf("%d", area);
    return 0;
}