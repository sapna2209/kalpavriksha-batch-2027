#include <stdio.h>

struct User {
    int id;
    char name[50];
    int age;
};

void addUser() {
    struct User u;

    FILE *fp = fopen("users.txt", "a");

    if (fp == NULL) {
        printf("File cannot be opened.\n");
        return;
    }

    printf("Enter ID: ");
    scanf("%d", &u.id);

    printf("Enter Name: ");
    scanf(" %[^\n]", u.name);

    printf("Enter Age: ");
    scanf("%d", &u.age);

    fprintf(fp, "%d\n", u.id);
    fprintf(fp, "%s\n", u.name);
    fprintf(fp, "%d\n", u.age);

    fclose(fp);

    printf("User added successfully.\n");
}


void showUsers() {
    struct User u;

    FILE *fp = fopen("users.txt", "r");

    if (fp == NULL) {
        printf("No records found.\n");
        return;
    }

    printf("\n----- USERS -----\n");

    while (fscanf(fp, "%d\n", &u.id) == 1) {

        fgets(u.name, 50, fp);

        fscanf(fp, "%d\n", &u.age);

        printf("\nID: %d\n", u.id);
        printf("Name: %s", u.name);
        printf("Age: %d\n", u.age);
    }

    fclose(fp);
}


void updateUser() {
    struct User u;
    int target;
    int found = 0;

    FILE *fp = fopen("users.txt", "r");
    FILE *temp = fopen("temp.txt", "w");

    if (fp == NULL) {
        printf("No records found.\n");
        return;
    }

    if (temp == NULL) {
        printf("Temporary file cannot be created.\n");
        fclose(fp);
        return;
    }

    printf("Enter ID to update: ");
    scanf("%d", &target);

    while (fscanf(fp, "%d\n", &u.id) == 1) {

        fgets(u.name, 50, fp);

        fscanf(fp, "%d\n", &u.age);

        if (u.id == target) {

            found = 1;

            printf("Enter new Name: ");
            scanf(" %[^\n]", u.name);

            printf("Enter new Age: ");
            scanf("%d", &u.age);
        }

        fprintf(temp, "%d\n", u.id);
        fprintf(temp, "%s", u.name);
        fprintf(temp, "%d\n", u.age);
    }

    fclose(fp);
    fclose(temp);

    remove("users.txt");
    rename("temp.txt", "users.txt");

    if (found)
        printf("User updated successfully.\n");
    else
        printf("ID not found.\n");
}


void deleteUser() {
    struct User u;
    int target;
    int found = 0;

    FILE *fp = fopen("users.txt", "r");
    FILE *temp = fopen("temp.txt", "w");

    if (fp == NULL) {
        printf("No records found.\n");
        return;
    }

    if (temp == NULL) {
        printf("Temporary file cannot be created.\n");
        fclose(fp);
        return;
    }

    printf("Enter ID to delete: ");
    scanf("%d", &target);

    while (fscanf(fp, "%d\n", &u.id) == 1) {

        fgets(u.name, 50, fp);

        fscanf(fp, "%d\n", &u.age);

        if (u.id == target) {
            found = 1;
            continue;
        }

        fprintf(temp, "%d\n", u.id);
        fprintf(temp, "%s", u.name);
        fprintf(temp, "%d\n", u.age);
    }

    fclose(fp);
    fclose(temp);

    remove("users.txt");
    rename("temp.txt", "users.txt");

    if (found)
        printf("User deleted successfully.\n");
    else
        printf("ID not found.\n");
}


int main() {

    int choice;

    do {
        printf("\n===== USER MANAGEMENT =====\n");
        printf("1. Add User\n");
        printf("2. Show Users\n");
        printf("3. Update User\n");
        printf("4. Delete User\n");
        printf("5. Exit\n");

        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice) {

            case 1:
                addUser();
                break;

            case 2:
                showUsers();
                break;

            case 3:
                updateUser();
                break;

            case 4:
                deleteUser();
                break;

            case 5:
                printf("Exiting...\n");
                break;

            default:
                printf("Invalid choice.\n");
        }

    } while (choice != 5);

    return 0;
}