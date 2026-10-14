#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define STANDARD_PRICE 150
#define PREMIUM_PRICE 250
#define RECLINER_PRICE 400

enum TicketCategory{
    STANDARD=1,
    PREMIUM,
    RECLINER
};

int isValidName(char name[]){
    int i=0;

    if (name[0]=='\0'){
        return 0;
    }

    while (name[i]!='\0'){
        if (!isalpha((unsigned char)name[i])){
            return 0;
        }
        i++;
    }

    return 1;
}

int main() {
    char name[50];
    char category[20];
    int tickets,choice,price;
    float subtotal,discount,finalBill;

    if (scanf("%49s",name)!=1||!isValidName(name)){
        printf("invalid customer name\n");
        return 0;
    }

    if (scanf("%d",&tickets)!=1||tickets<1||tickets>20){
        printf("invalid ticket count\n");
        return 0;
    }

    if (scanf("%d",&choice)!=1){
        printf("invalid ticket category\n");
        return 0;
    }

    switch(choice){
        case STANDARD:
            strcpy(category,"Standard");
            price=STANDARD_PRICE;
            break;

        case PREMIUM:
            strcpy(category,"Premium");
            price=PREMIUM_PRICE;
            break;

        case RECLINER:
            strcpy(category,"Recliner");
            price=RECLINER_PRICE;
            break;

        default:
            printf("invalid ticket category\n");
            return 0;
    }

    subtotal=tickets*price;

    if (tickets>=5){
        discount=subtotal*0.10f;
    } else{
        discount=0.0f;
    }

    finalBill=subtotal-discount;

    printf("Customer: %s\n",name);
    printf("Ticket Category: %s\n",category);
    printf("Tickets: %d\n",tickets);
    printf("Subtotal: %.2f\n",subtotal);
    printf("Discount: %.2f\n",discount);
    printf("Final Bill: %.2f\n",finalBill);

    return 0;
}