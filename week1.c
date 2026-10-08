#include <stdio.h>

#define MAX 100

struct Student {
    int roll;
    char name[50];
    int marks[3];
};

int totalStudents = 0;

int getTotal(struct Student s)
{
    int sum = 0;
    int i;

    for (i = 0; i < 3; i++) {
        sum += s.marks[i];
    }

    return sum;
}

float getAverage(int total)
{
    return total / 3.0;
}

char getGrade(float avg)
{
    if (avg >= 85)
        return 'A';
    if (avg >= 70)
        return 'B';
    if (avg >= 50)
        return 'C';
    if (avg >= 35)
        return 'D';

    return 'F';
}

int getStarCount(char grade)
{
    if (grade == 'A')
        return 5;
    if (grade == 'B')
        return 4;
    if (grade == 'C')
        return 3;
    if (grade == 'D')
        return 2;

    return 0;
}

void showPattern(int count)
{
    int i;

    for (i = 1; i <= count; i++) {
        printf("*");
    }

    printf("\n");
}

void showRollNumbers(struct Student s[], int position)
{
    if (position >= totalStudents)
        return;

    printf("%d", s[position].roll);

    if (position < totalStudents - 1)
        printf(" ");

    showRollNumbers(s, position + 1);
}

int main()
{
    struct Student s[MAX];
    int n;
    int i;
    int j;

    scanf("%d", &n);

    if (n < 1 || n > MAX) {
        printf("Invalid number of students.\n");
        return 0;
    }

    totalStudents = n;

    for (i = 0; i < n; i++) {
        if (scanf("%d %49s %d %d %d",
                  &s[i].roll,
                  s[i].name,
                  &s[i].marks[0],
                  &s[i].marks[1],
                  &s[i].marks[2]) != 5) {
            printf("Invalid input. Please enter Roll Number, Name and 3 marks correctly.\n");
            return 0;
        }

        for (j = 0; j < 3; j++) {
            if (s[i].marks[j] < 0 || s[i].marks[j] > 100) {
                printf("Invalid marks. Marks must be between 0 and 100.\n");
                return 0;
            }
        }
    }

    for (i = 0; i < n; i++) {
        int total = getTotal(s[i]);
        float average = getAverage(total);
        char grade = getGrade(average);

        printf("Roll: %d\n", s[i].roll);
        printf("Name: %s\n", s[i].name);
        printf("Total: %d\n", total);
        printf("Average: %.2f\n", average);
        printf("Grade: %c\n", grade);

        if (grade == 'F')
            continue;

        printf("Performance: ");
        showPattern(getStarCount(grade));
    }

    printf("List of Roll Numbers (via recursion): ");
    showRollNumbers(s, 0);
    printf("\n");

    return 0;
}