#include <stdio.h>
#include <string.h>
#include <ctype.h>

struct User {
    int id;
    char name[50];
    int age;
};

int isValidName(char name[]) {
    int i = 0;
    int hasLetter = 0;

    while (name[i] != '\0') {
        if (isalpha((unsigned char)name[i])) {
            hasLetter = 1;
        } else if (name[i] != ' ') {
            return 0;
        }

        i++;
    }

    return hasLetter;
}

void inputName(char name[]) {
    do {
        printf("Enter Name: ");
        fgets(name, sizeof(name), stdin);

        name[strcspn(name, "\n")] = '\0';

        if (!isValidName(name)) {
            printf("Invalid name. Name should contain only letters and spaces.\n");
        }

    } while (!isValidName(name));
}

void inputAge(int *age) {
    char input[20];

    while (1) {
        printf("Enter Age: ");
        fgets(input, sizeof(input), stdin);

        if (sscanf(input, "%d", age) == 1 && *age >= 1 && *age <= 120) {
            break;
        }

        printf("Invalid age. Please enter an age between 1 and 120.\n");
    }
}

void addUser() {
    struct User u;
    FILE *fp = fopen("users.txt", "a");

    if (fp == NULL) {
        printf("File cannot be opened.\n");
        return;
    }

    printf("Enter ID: ");
    scanf("%d", &u.id);
    getchar();

    inputName(u.name);
    inputAge(&u.age);

    fprintf(fp, "%d\n", u.id);
    fprintf(fp, "%s\n", u.name);
    fprintf(fp, "%d\n", u.age);

    fclose(fp);

    printf("User added successfully.\n");
}

void showUsers() {
    struct User u;
    char line[20];

    FILE *fp = fopen("users.txt", "r");

    if (fp == NULL) {
        printf("No records found.\n");
        return;
    }

    printf("\n------------------------------------------------\n");
    printf("%-10s %-25s %-10s\n", "ID", "Name", "Age");
    printf("------------------------------------------------\n");

    while (fgets(line, sizeof(line), fp) != NULL) {
        sscanf(line, "%d", &u.id);

        if (fgets(u.name, sizeof(u.name), fp) == NULL) {
            break;
        }

        u.name[strcspn(u.name, "\n")] = '\0';

        if (fgets(line, sizeof(line), fp) == NULL) {
            break;
        }

        sscanf(line, "%d", &u.age);

        printf("%-10d %-25s %-10d\n", u.id, u.name, u.age);
    }

    printf("------------------------------------------------\n");

    fclose(fp);
}

void updateUser() {
    struct User u;
    char line[20];
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
    getchar();

    while (fgets(line, sizeof(line), fp) != NULL) {
        sscanf(line, "%d", &u.id);

        if (fgets(u.name, sizeof(u.name), fp) == NULL) {
            break;
        }

        u.name[strcspn(u.name, "\n")] = '\0';

        if (fgets(line, sizeof(line), fp) == NULL) {
            break;
        }

        sscanf(line, "%d", &u.age);

        if (u.id == target) {
            found = 1;

            printf("\nEnter new details:\n");

            inputName(u.name);
            inputAge(&u.age);
        }

        fprintf(temp, "%d\n", u.id);
        fprintf(temp, "%s\n", u.name);
        fprintf(temp, "%d\n", u.age);
    }

    fclose(fp);
    fclose(temp);

    remove("users.txt");
    rename("temp.txt", "users.txt");

    if (found) {
        printf("User updated successfully.\n");
    } else {
        printf("ID not found.\n");
    }
}

void deleteUser() {
    struct User u;
    char line[20];
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
    getchar();

    while (fgets(line, sizeof(line), fp) != NULL) {
        sscanf(line, "%d", &u.id);

        if (fgets(u.name, sizeof(u.name), fp) == NULL) {
            break;
        }

        u.name[strcspn(u.name, "\n")] = '\0';

        if (fgets(line, sizeof(line), fp) == NULL) {
            break;
        }

        sscanf(line, "%d", &u.age);

        if (u.id == target) {
            found = 1;
            continue;
        }

        fprintf(temp, "%d\n", u.id);
        fprintf(temp, "%s\n", u.name);
        fprintf(temp, "%d\n", u.age);
    }

    fclose(fp);
    fclose(temp);

    remove("users.txt");
    rename("temp.txt", "users.txt");

    if (found) {
        printf("User deleted successfully.\n");
    } else {
        printf("ID not found.\n");
    }
}

void menu() {
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
        getchar();

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
}

int main() {
    menu();

    return 0;
}
