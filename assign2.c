
#include <stdio.h>
#include <string.h>
#include <ctype.h>

struct User {
    int id;
    char name[50];
    int age;
};

int isValidName(char name[]) {
    int i = 0, hasLetter = 0;

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

int inputInt(int *value) {
    char input[100], extra;

    while (1) {
        if (fgets(input, sizeof(input), stdin) == NULL)
            return 0;

        if (strchr(input, '\n') == NULL && !feof(stdin)) {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF);
            printf("Invalid input. Enter a number: ");
            continue;
        }

        if (sscanf(input, " %d %c", value, &extra) == 1)
            return 1;

        printf("Invalid input. Enter a number: ");
    }
}

int idExists(int id) {
    FILE *fp = fopen("users.txt", "r");
    int fid, age;
    char name[50];

    if (fp == NULL)
        return 0;

    while (fscanf(fp, "%d\n", &fid) == 1) {
        if (fgets(name, sizeof(name), fp) == NULL)
            break;

        name[strcspn(name, "\n")] = '\0';

        if (fscanf(fp, "%d\n", &age) != 1)
            break;

        if (fid == id) {
            fclose(fp);
            return 1;
        }
    }

    fclose(fp);
    return 0;
}

int inputName(char name[]) {
    while (1) {
        printf("Enter Name: ");

        if (fgets(name, 50, stdin) == NULL)
            return 0;

        if (strchr(name, '\n') == NULL && !feof(stdin)) {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF);
            printf("Name is too long. Try again.\n");
            continue;
        }

        name[strcspn(name, "\n")] = '\0';

        if (isValidName(name))
            return 1;

        printf("Invalid name. Use only letters and spaces.\n");
    }
}

int inputAge(int *age) {
    while (1) {
        printf("Enter Age: ");

        if (!inputInt(age))
            return 0;

        if (*age >= 1 && *age <= 120)
            return 1;

        printf("Age must be between 1 and 120.\n");
    }
}

int readUser(FILE *fp, struct User *u) {
    if (fscanf(fp, "%d\n", &u->id) != 1)
        return 0;

    if (fgets(u->name, sizeof(u->name), fp) == NULL)
        return 0;

    if (strchr(u->name, '\n') == NULL && !feof(fp))
        return 0;

    u->name[strcspn(u->name, "\n")] = '\0';

    if (fscanf(fp, "%d\n", &u->age) != 1)
        return 0;

    return 1;
}

int writeUser(FILE *fp, struct User u) {
    return fprintf(fp, "%d\n%s\n%d\n",
                   u.id, u.name, u.age) >= 0;
}

int inputDetails(struct User *u) {
    if (!inputName(u->name))
        return 0;

    if (!inputAge(&u->age))
        return 0;

    return 1;
}

void addUser() {
    struct User u;
    FILE *fp;

    while (1) {
        printf("Enter ID: ");

        if (!inputInt(&u.id))
            return;

        if (u.id <= 0)
            printf("ID must be positive.\n");
        else if (idExists(u.id))
            printf("ID already exists. Enter another ID.\n");
        else
            break;
    }

    if (!inputDetails(&u))
        return;

    fp = fopen("users.txt", "a");

    if (fp == NULL) {
        printf("File cannot be opened.\n");
        return;
    }

    if (!writeUser(fp, u) || fclose(fp) != 0) {
        printf("Error saving user.\n");
        return;
    }

    printf("User added successfully.\n");
}

void showUsers() {
    struct User u;
    FILE *fp = fopen("users.txt", "r");
    int found = 0;

    if (fp == NULL) {
        printf("No records found.\n");
        return;
    }

    printf("\n------------------------------------------------\n");
    printf("%-10s %-25s %-10s\n", "ID", "Name", "Age");
    printf("------------------------------------------------\n");

    while (readUser(fp, &u)) {
        printf("%-10d %-25s %-10d\n", u.id, u.name, u.age);
        found = 1;
    }

    if (!found)
        printf("No records found.\n");

    printf("------------------------------------------------\n");
    fclose(fp);
}

int replaceFile(int found, int error) {
    if (error || !found) {
        remove("temp.txt");
        if (!error)
            printf("ID not found.\n");
        return 0;
    }

    if (rename("temp.txt", "users_new.txt") != 0) {
        printf("Error preparing updated file.\n");
        remove("temp.txt");
        return 0;
    }

    if (remove("users.txt") != 0) {
        printf("Error replacing original file.\n");
        return 0;
    }

    if (rename("users_new.txt", "users.txt") != 0) {
        printf("Could not restore the file name.\n");
        return 0;
    }

    return 1;
}

void updateUser() {
    struct User u;
    FILE *fp = fopen("users.txt", "r");
    FILE *temp;
    int target, found = 0, error = 0;

    if (fp == NULL) {
        printf("No records found.\n");
        return;
    }

    printf("Enter ID to update: ");

    if (!inputInt(&target)) {
        fclose(fp);
        return;
    }

    temp = fopen("temp.txt", "w");

    if (temp == NULL) {
        fclose(fp);
        printf("Temporary file cannot be created.\n");
        return;
    }

    while (readUser(fp, &u)) {
        if (u.id == target) {
            found = 1;
            printf("Enter new details:\n");

            if (!inputDetails(&u)) {
                error = 1;
                break;
            }
        }

        if (!writeUser(temp, u)) {
            error = 1;
            break;
        }
    }

    if (ferror(fp))
        error = 1;

    fclose(fp);

    if (fclose(temp) != 0)
        error = 1;

    if (replaceFile(found, error))
        printf("User updated successfully.\n");
    else if (error)
        printf("Update failed.\n");
}

void deleteUser() {
    struct User u;
    FILE *fp = fopen("users.txt", "r");
    FILE *temp;
    int target, found = 0, error = 0;

    if (fp == NULL) {
        printf("No records found.\n");
        return;
    }

    printf("Enter ID to delete: ");

    if (!inputInt(&target)) {
        fclose(fp);
        return;
    }

    temp = fopen("temp.txt", "w");

    if (temp == NULL) {
        fclose(fp);
        printf("Temporary file cannot be created.\n");
        return;
    }

    while (readUser(fp, &u)) {
        if (u.id == target) {
            found = 1;
            continue;
        }

        if (!writeUser(temp, u)) {
            error = 1;
            break;
        }
    }

    if (ferror(fp))
        error = 1;

    fclose(fp);

    if (fclose(temp) != 0)
        error = 1;

    if (replaceFile(found, error))
        printf("User deleted successfully.\n");
    else if (error)
        printf("Delete failed.\n");
}

void menu() {
    int choice;

    while (1) {
        printf("\n===== USER MANAGEMENT =====\n");
        printf("1. Add User\n");
        printf("2. Show Users\n");
        printf("3. Update User\n");
        printf("4. Delete User\n");
        printf("5. Exit\n");
        printf("Enter choice: ");

        if (!inputInt(&choice))
            break;

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
                return;
            default:
                printf("Invalid choice.\n");
        }
    }
}

int main() {
    menu();
    return 0;
}
