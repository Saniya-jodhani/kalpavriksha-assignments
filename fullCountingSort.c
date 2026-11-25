void countSort(int arr_rows, int arr_columns, char*** arr) {
    int key[arr_rows];
    for(int i=0; i < arr_rows;i++){
        key[i] = atoi(arr[i][0]);
    }
    int first = 1;
    for(int k=0; k < 100;k++){
        for(int i=0; i < arr_rows ;i++){
            if(key[i]==k){
                if(!first) printf(" ");
                if(i < arr_rows/2) printf("-");
                else printf("%s", arr[i][1]);
                
                first =0;
            }
        }
    }
    printf("\n");
}
