#include <stdio.h>
struct Student {
    int roll;
    char name[50];
    int marks1;
    int marks2;
    int marks3;

};

int calculateTotal(struct Student s) {
    return s.marks1 + s.marks2 + s.marks3;
}

float calculateAverage(int total) {
    return total/3.0;
}

char calculateGrade(float average) {
    if(average >= 85){
        return 'A';
    } else if(average >=70){
        return 'B';
    }else if(average >=50){
        return 'C';
    } else if(average >=35){
        return 'D';
    } else{
        return 'F';
    }
}

void printPerformance(char grade){
    int stars;
    if(grade =='A') stars = 5;
    else if(grade == 'B' )stars = 4;
    else if(grade=='C') stars = 3;
    else stars = 2;
    
    for(int i =0; i < stars; i++){
        printf("*");
    }
    printf("\n");
}

void printRollNumber(int n , int current){
    if(current > n)return;
    printf("%d ", current);
    printRollNumber(n , current +1);

}

int main() {
    int n ;
    printf("Enter the number of students:");
    scanf("%d",&n);

    struct Student students[100];

    for(int i=0; i < n;i++){
        printf("\n Enter details for student %d:\n", i+1);
        scanf("%d %s %d %d %d", &students[i].roll, students[i].name, &students[i].marks1, &students[i].marks2, &students[i].marks3);
    }
        for(int i=0; i < n;i++){
            int total = calculateTotal(students[i]);
            float average = calculateAverage(total);
            char grade = calculateGrade(average);

            printf("\n Roll no. %d\n", students[i].roll);
            printf("Name: %s\n", students[i].name);
            printf("Total: %d\n", total);
            printf("Average: %.2f\n", average);
            printf("Grade: %c\n", grade);

            if(average < 35) continue;

            printf("Performance: ");
            printPerformance(grade);
        }
        printf("\n List of Roll Numbers:\n");
        printRollNumber(n, 1);
        printf("\n");

        return 0;
    
}