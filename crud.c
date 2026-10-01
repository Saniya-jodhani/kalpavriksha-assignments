#include <stdio.h>
#include <string.h>
struct User {
    int id;
    char name[50];
    int age;
};

void createFile() {
    FILE *fp = fopen("users.txt","a");
    if(fp!=NULL) fclose(fp);
}

int idExist(int id){
    FILE *fp = fopen("users.txt","r");
    struct User u;
    if(fp == NULL ) return 0;
    while(fscanf(fp,"%d,%49[^,],%d\n", &u.id, u.name, &u.age)==3){
        if(u.id == id){
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

void createUser() {
    FILE *fp = fopen("users.txt","a");
    struct User u;
    printf("Enter ID: ");
    scanf("%d", &u.id);
    if(idExist(u.id)){
        printf("ID already exists.\n");
        fclose(fp);
        return;
    }
    printf("Enter Name : ");
    scanf(" %49[^\n]", u.name);

    printf("Enter Age : ");
    scanf("%d", &u.age);

    fprintf(fp,"%d,%s,%d\n", u.id, u.name , u.age);
    fclose(fp);
    printf("User is added!");
}

void readUser(){
    FILE *fp = fopen("users.txt","r");
    struct User u;
    if(fp==NULL){
        printf("No records found.\n");
        return;
    }
    printf("\nID\tName\tage\n");
    while(fscanf(fp,"%d,%49[^,],%d\n", &u.id, u.name, &u.age)==3){
        printf("%d\t%s\t%d\n", u.id, u.name , u.age);
    }
    fclose(fp);
}

void updateUser(){
    FILE *fp = fopen("users.txt","r");
    FILE *temp = fopen("temp.txt","w");

    struct User u;
    int id , found =0;
    if(fp==NULL || temp ==NULL){
        printf("File Error.");
        if(fp!=NULL) fclose(fp);
        if(temp!=NULL) fclose(temp);
        return;
    }
    printf("Enter ID to update: ");
    scanf("%d",&id);

    while(fscanf(fp,"%d, %49[^,],%d\n", &u.id , u.name , &u.age)==3){
        if(u.id ==id){
            found =1;
            printf("Enter new Name: ");
            scanf(" %49[^\n]", u.name);
            printf("Enter new Age : ");
            scanf("%d",&u.age);
        }
        fprintf(temp,"%d,%s,%d\n", u.id, u.name , u.age);
    }
    fclose(fp);
    fclose(temp);
    remove("users.txt");
    rename("temp.txt", "users.txt");

    if(found) printf("User is updated!\n");
    else printf("User is not found.\n");
}

void deleteUser(){
    FILE *fp = fopen("users.txt","r");
    FILE *temp = fopen("temp.txt","w");
    struct User u;
    int id , found=0;
    if(fp==NULL || temp ==NULL){
        printf("File Error.");
        if(fp!=NULL) fclose(fp);
        if(temp!=NULL) fclose(temp);
        return;
    }
    printf("Enter ID to delete: ");
    scanf("%d",&id);

    while(fscanf(fp, "%d,%49[^,],%d\n", &u.id , u.name , &u.age)==3){
        if(u.id==id){
            found =1;
            continue;
        }
        fprintf(temp, "%d, %s, %d\n", u.id , u.name , u.age);
    }
    fclose(fp);
    fclose(temp);

    remove("users.txt");
    rename("temp.txt","users.txt");

    if(found) printf("User is deleted.\n");
    else printf("Uset not found\n");
}

int main(){
    int choice;
    createFile();

    do{
        printf("\n 1. Create User\n");
        printf("\n 2. Read users \n");
        printf("\n 3. Update users \n");
        printf("\n 4. Delete users \n");
        printf("\n 5. Exit \n");
        printf("Enter choice : ");
        scanf("%d", &choice);

        switch(choice){
            case 1: createUser(); break;
            case 2: readUser();break;
            case 3: updateUser();break;
            case 4: deleteUser();break;
            case 5: printf("Exit...\n"); break;
            default: printf("Invalid choice.\n");
        }
        
    }while(choice !=5);
    return 0;
}