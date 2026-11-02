#include<stdio.h>
#include<string.h>
#include<stdlib.h>

int* matchingStrings(int stringList_count, char** stringList, int queries_count, char** queries, int* result_count) {
    int *result = malloc(queries_count*sizeof(int));
    *result_count = queries_count;
    
    for(int i=0; i < queries_count;i++){
        result[i] = 0;
        for(int j=0; j < stringList_count;j++){
            if(strcmp(queries[i],stringList[j])==0){
                result[i]++;
            }
        }
    }
    return result;
}

int main(){
    int n , q;

    scanf("%d", &n);
    char *stringLast[n];

    for(int i=0; i < n;i++){
        stringLast[i] = malloc(21*sizeof(char));
        scanf("%s",stringLast[i]);
    }

    scanf("%d", &q);
    char *queries[q];   

    for(int i=0; i < q;i++){
        queries[i] = malloc(21*sizeof(char));
        scanf("%s",queries[i]);
    }       

    int result_count;
    int *result = matchingStrings(n,stringLast,q,queries,&result_count);


    for(int i=0; i < result_count;i++){
        printf("%d\n",result[i]);
    }

    return 0;
}