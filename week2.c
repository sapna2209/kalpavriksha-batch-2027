#include <stdio.h>

#define STANDARD_PRICE 150
#define PREMIUM_PRICE 250
#define RECLINER_PRICE 400

enum TicketCategory {
    STANDARD=1,
    PREMIUM=2,
    RECLINER=3
};

struct Customer {
    char name[50];
    int tickets;
    enum TicketCategory category;
};

struct Bill {
    int price;
    float subtotal;
    float discount;
    float total;
};
int isValidName(char name[]);
int selectCategory(void);
void calculateBill(struct Customer *customer,struct Bill *bill);
void displayBill(struct Customer customer,struct Bill bill);

int main(void) {
    struct Customer customer;
    struct Bill bill;
    if(scanf("%49s",customer.name)!=1||!isValidName(customer.name)){
        printf("Invalid customer name\n");
        return 1;
    }
    if(scanf("%d",&customer.tickets)!=1||customer.tickets<1||customer.tickets>20){
        printf("Invalid ticket count\n");
        return 1;
    }
    customer.category=selectCategory();
    if(customer.category==0){
        printf("Invalid ticket category\n");
        return 1;
    }
    calculateBill(&customer,&bill);
    displayBill(customer,bill);

    return 0;
}

int isValidName(char name[]) {
    int i=0;
    if (name[0]=='\0')return 0;
    while (name[i]!='\0') {
        if(!((name[i]>='A'&&name[i]<='Z')||(name[i]>='a'&&name[i]<='z'))){
            return 0;
        }i++;}
    return 1;
}
int selectCategory(void) {
    int choice;
    if (scanf("%d",&choice)!=1)return 0;
    switch (choice) {
        case STANDARD:
        case PREMIUM:
        case RECLINER:
            return choice;
        default:
            return 0;}
}
void calculateBill(struct Customer *customer,struct Bill *bill) {
    switch(customer->category){
        case STANDARD:
            bill->price=STANDARD_PRICE;
            break;
        case PREMIUM:
            bill->price=PREMIUM_PRICE;
            break;
        case RECLINER:
            bill->price=RECLINER_PRICE;
            break;
        default:
            bill->price=0;
    }

    bill->subtotal=customer->tickets*bill->price;

    if (customer->tickets>=5)bill->discount=bill->subtotal*0.10f;
    else bill->discount=0.0f;

    bill->total=bill->subtotal-bill->discount;
}

void displayBill(struct Customer customer,struct Bill bill) {
    printf("Customer: %s\n",customer.name);
    switch(customer.category){
        case STANDARD:
            printf("Ticket Category: Standard\n");
            break;
        case PREMIUM:
            printf("Ticket Category: Premium\n");
            break;
        case RECLINER:
            printf("Ticket Category: Recliner\n");
            break;
    }
    printf("Tickets: %d\n",customer.tickets);
    printf("Subtotal: %.2f\n",bill.subtotal);
    printf("Discount: %.2f\n",bill.discount);
    printf("Final Bill: %.2f\n",bill.total);
}
