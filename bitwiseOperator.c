#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>



void calculate_the_maximum(int n, int k) {
  int max_and =0;
    int max_or =0;
    int max_xor =0;
    
    for(int a =1 ; a <= k; a++){
        for(int b = a+1; b <=n;b++){
            int andVal = a & b;
            int orVal = a | b;
            int xorVal = a ^ b;
            
            if(andVal < k && andVal > max_and){
                max_and = andVal;
            }
            if(orVal < k && orVal > max_or){
                max_or = orVal;
            }
            if(xorVal < k && xorVal > max_xor){
                max_xor = xorVal;
            }
        }
    }
    
    printf("%d\n",max_and);
    printf("%d\n",max_or);
    printf("%d",max_xor);
}

int main() {
    int n, k;
  
    scanf("%d %d", &n, &k);
    calculate_the_maximum(n, k);
    
    
    
 
    return 0;
}
