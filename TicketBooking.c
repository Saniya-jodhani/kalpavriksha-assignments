#include <stdio.h>
#include <string.h>
int main() {
    char customerName[50];
    int tickets, choice;
    const float STANDARD = 150.0;
    const float PREMIUM = 250.0;
    const float RECLINER = 400.0;

    float ticketPrice, subtotal, discount , finalBill;
    char category[20];

    printf("Enter Customer Name: ");
    scanf("%49s", customerName);
    printf("Enter number of tickets (1-20): ");
    scanf("%d", &tickets);
    
    if(tickets <=1 || tickets > 20){
        printf("Invalid ticket count");
        return 0;
    }
    printf("Select ticket category:\n");
    printf("1. Standard - Rs. 150\n");
    printf("2. Premium - Rs. 250\n");
    printf("3. Recliner - Rs. 400\n");
    printf("Enter your choice (1-3): ");
    scanf("%d", &choice);
    

    
    switch(choice){
        case 1:
        ticketPrice = STANDARD;
        strcpy(category,"Standard");
        break;

        case 2:
        ticketPrice = PREMIUM;
        strcpy(category,"Premium");break;

        case 3:
        ticketPrice = RECLINER;
        strcpy(category , "Recliner"); break;

        default:
        printf("Invalid ticket Category");
        return 0;
    }

    subtotal= ticketPrice * tickets;

    if(tickets >=5){
        discount = subtotal * 0.1;
    } else{
        discount = 0.0;
    }
    finalBill= subtotal - discount;

    printf("Customer: %s\n",customerName);
    printf("Category: %s\n",category);
    printf("Tickets: %d\n",tickets);
    printf("Subtotal: %.2f\n",subtotal);    
    printf("Discount: % .2f\n", discount);
    printf("Final Bill : %.2f\n", finalBill);

    return 0;
}