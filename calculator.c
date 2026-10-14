#include <stdio.h>
#include <ctype.h>
#include <string.h>

int main() {
    char exp[100];
    int nums[50];
    char op[50];
    int numCount =0;
    int opCount = 0;
    int res =0;
    int num =0;

    printf("Enter the expression: ");
    fgets(exp, sizeof(exp), stdin);

    for(int i=0; exp[i] != '\0'; i++){
        if(isdigit(exp[i])){
            num = num *10 +(exp[i] -'0');
        }

        if(exp[i] == '+' || exp[i] == '-' || exp[i] == '*' || exp[i] =='/'){
            op[opCount] = exp[i];
            opCount++;

            nums[numCount] = num;
            numCount ++;
            num =0;
        }
    }

    nums[numCount]  = num;
    numCount++;

    int last = strlen(exp) - 1;

    while(last >=0 && isspace(exp[last])){
        last--;
    }
    if(exp[last] =='+' || exp[last] =='-' || exp[last] == '*' || exp[last] =='/'){
        printf("Error:Invalid Expression.");
        return 0;
    }

    for(int i=0; i < opCount; i++){
        if(op[i]=='*' || op[i] =='/'){
            if(op[i] =='*'){
                nums[i] = nums[i] * nums[i+1];
            } else {
                if(nums[i+1] ==0){
                    printf("Error: Division by zero.");
                    return 0;
                }
                nums[i] = nums[i] / nums[i+1];
            }


        for(int j =i ; j < opCount-1; j++){
            op[j] = op[j+1];
        }
        opCount--;
        for(int j=i+1; j < numCount-1;j++){
            nums[j] = nums[j+1];
        }
        numCount--;
        i--;
    }
    }
    res = nums[0];
    for(int i=0; i < opCount; i++){
        if(op[i] == '+'){
            res = res + nums[i+1];
        } else if(op[i]=='-'){
            res = res - nums[i+1];
        }
    }
    printf("%d",res);
    return 0;
}