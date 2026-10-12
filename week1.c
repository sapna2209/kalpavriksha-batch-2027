#include <stdio.h>
#include <ctype.h>

#define MAX 100
#define NAME_SIZE 50
#define SUBJECTS 3
#define MIN_MARKS 0
#define MAX_MARKS 100

struct Student {
    int roll;
    char name[NAME_SIZE];
    int marks[SUBJECTS];
};

int totalStudents = 0;

int getTotal(struct Student s);
float getAverage(int total);
char getGrade(float avg);
int getStarCount(char grade);
void showPattern(int count);
void showRollNumbers(struct Student s[],int position);
int isValidName(char name[]);
int isRollExists(struct Student s[],int count,int roll);

int getTotal(struct Student s)
{
    int sum=0;
    int i;

    for (i=0;i<SUBJECTS;i++) {
        sum+=s.marks[i];
    }

    return sum;
}

float getAverage(int total)
{
    return total/(float)SUBJECTS;
}

char getGrade(float avg)
{
    if (avg>=85)
        return 'A';
    else if (avg>=70)
        return 'B';
    else if (avg>=50)
        return 'C';
    else if (avg>=35)
        return 'D';
    else
        return 'F';
}

int getStarCount(char grade)
{
    switch (grade) {
        case 'A':
            return 5;
        case 'B':
            return 4;
        case 'C':
            return 3;
        case 'D':
            return 2;
        default:
            return 0;
    }
}

void showPattern(int count)
{
    int i;

    for (i=0;i<count;i++) {
        printf("*");
    }

    printf("\n");
}

void showRollNumbers(struct Student s[],int position)
{
    if (position >= totalStudents)
        return;

    printf("%d",s[position].roll);

    if (position<totalStudents-1)
        printf(" ");

    showRollNumbers(s,position+1);
}

int isValidName(char name[])
{
    int i;

    if (name[0]=='\0')
        return 0;

    for (i=0;name[i]!='\0';i++) {
        if (!isalpha((unsigned char)name[i]))
            return 0;
    }

    return 1;
}

int isRollExists(struct Student s[],int count,int roll)
{
    int i;

    for (i=0;i<count;i++) {
        if (s[i].roll==roll)
            return 1;
    }

    return 0;
}

int main(void)
{
    struct Student s[MAX];
    int n;
    int i, j;

    if (scanf("%d",&n)!=1) {
        printf("invalid input\n");
        return 0;
    }

    if (n<1||n>MAX) {
        printf("invalid number of students\n");
        return 0;
    }

    totalStudents = n;

    for (i=0;i<n;i++) {
        if (scanf("%d %49s %d %d %d",
                  &s[i].roll, s[i].name,
                  &s[i].marks[0], &s[i].marks[1],
                  &s[i].marks[2]) != 5) {
            printf("invalid input\n");
            return 0;
        }

        if (s[i].roll<=0) {
            printf("invalid roll number\n");
            return 0;
        }

        if (isRollExists(s,i,s[i].roll)) {
            printf("Roll number already exists.\n");
            return 0;
        }

        if (!isValidName(s[i].name)) {
            printf("Invalid name. Use alphabets only.\n");
            return 0;
        }

        for (j=0;j<SUBJECTS;j++) {
            if (s[i].marks[j]<MIN_MARKS||s[i].marks[j]>MAX_MARKS) {
                printf("invalid marks, marks must be between %d and %d\n",MIN_MARKS, MAX_MARKS);
                return 0;
            }
        }
    }

    for (i=0;i<n;i++) {
        int total=getTotal(s[i]);
        float average=getAverage(total);
        char grade=getGrade(average);

        printf("roll: %d\n",s[i].roll);
        printf("name: %s\n",s[i].name);
        printf("total: %d\n",total);
        printf("average: %.2f\n",average);
        printf("grade: %c\n",grade);

        if (grade=='F')
            continue;

        printf("performance: ");
        showPattern(getStarCount(grade));
    }

    printf("List of Roll Numbers (via recursion): ");
    showRollNumbers(s,0);
    printf("\n");

    return 0;
}
