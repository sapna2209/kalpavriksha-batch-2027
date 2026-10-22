#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define MAX_INPUT_LENGTH 50

enum MenuOption{
    INVALID_CHOICE=0,
    ADD_EMPLOYEE=1,
    REMOVE_EMPLOYEE=2,
    SHOW_EMPLOYEE_HIERARCHY=3,
    SHOW_COMPLETE_HIERARCHY=4,
    EXIT_PROGRAM=5
};

typedef struct Employee Employee;

struct Employee{
    int id;
    char name[MAX_INPUT_LENGTH];
    char designation[MAX_INPUT_LENGTH];
    Employee *manager;
    Employee *firstReport;
    Employee *nextSibling;
};

int readInteger(int *value);
int readText(char text[],int size);
int isValidText(const char text[]);
Employee *findEmployee(Employee *root,int id);
Employee *readExistingEmployee(Employee *ceo,const char prompt[]);
int readEmployeeDetails(Employee *ceo,Employee *input,int *managerId);
void addReport(Employee *manager,Employee *employee);
Employee *addEmployee(Employee *ceo);
int countDirectReports(const Employee *employee);
Employee *chooseNewParent(const Employee *employee);
void detachFromManager(Employee *employee);
void transferReports(Employee *employee,Employee *newParent);
Employee *removeEmployee(Employee *ceo);
int countReports(const Employee *employee);
void displayEmployees(const Employee *employee);
void displayEmployeeHierarchy(Employee *ceo);
void displayCompleteHierarchy(const Employee *ceo);
void freeHierarchy(Employee *employee);
void displayMenu(void);

int readInteger(int *value){
    char input[MAX_INPUT_LENGTH];
    int i=0,number=0,ch;

    while((ch=getchar())!='\n'&&ch!=EOF){
        if(i>=MAX_INPUT_LENGTH-1){
            while(ch!='\n'&&ch!=EOF)
                ch=getchar();
            return 0;
        }

        input[i]=(char)ch;
        i++;
    }

    input[i]='\0';

    if(i==0)
        return 0;

    for(int j=0;j<i;j++){
        int digit;

        if(input[j]<'0'||input[j]>'9')
            return 0;

        digit=input[j]-'0';

        if(number>(INT_MAX-digit)/10)
            return 0;

        number=number*10+digit;
    }

    *value=number;
    return 1;
}

int readText(char text[],int size){
    int i=0,ch;

    if(fgets(text,size,stdin)==NULL)
        return 0;

    while(text[i]!='\0'&&text[i]!='\n')
        i++;

    if(text[i]=='\n'){
        text[i]='\0';
    }
    else if(i==size-1){
        ch=getchar();

        if(ch!='\n'&&ch!=EOF){
            while(ch!='\n'&&ch!=EOF)
                ch=getchar();

            return 0;
        }
    }

    return 1;
}

int isValidText(const char text[]){
    int i=0,hasLetter=0;

    if(text[0]=='\0'||text[0]==' ')
        return 0;

    while(text[i]!='\0'){
        if((text[i]>='A'&&text[i]<='Z')||
           (text[i]>='a'&&text[i]<='z')){
            hasLetter=1;
        }
        else if(text[i]==' '||text[i]=='-'){
            if(i==0||text[i-1]==' '||text[i-1]=='-')
                return 0;

            if(text[i+1]=='\0')
                return 0;
        }
        else{
            return 0;
        }

        i++;
    }

    return hasLetter;
}

Employee *findEmployee(Employee *root,int id){
    Employee *report,*found;

    if(root==NULL)
        return NULL;

    if(root->id==id)
        return root;

    for(report=root->firstReport;report!=NULL;report=report->nextSibling){
        found=findEmployee(report,id);

        if(found!=NULL)
            return found;
    }

    return NULL;
}

Employee *readExistingEmployee(Employee *ceo,const char prompt[]){
    Employee *employee;
    int id;

    if(ceo==NULL){
        printf("No employees in the organization\n");
        return NULL;
    }

    printf("%s",prompt);

    if(!readInteger(&id)||id<=0){
        printf("Invalid employee ID\n");
        return NULL;
    }

    employee=findEmployee(ceo,id);

    if(employee==NULL)
        printf("Employee not found\n");

    return employee;
}

int readEmployeeDetails(Employee *ceo,Employee *input,int *managerId){
    if(ceo==NULL)
        printf("No employees yet. This employee will be the CEO.\n");

    printf("Enter Employee ID: ");

    if(!readInteger(&input->id)||input->id<1){
        printf("Invalid employee ID\n");
        return 0;
    }

    if(findEmployee(ceo,input->id)!=NULL){
        printf("Employee ID already exists\n");
        return 0;
    }

    printf("Enter Name: ");

    if(!readText(input->name,sizeof(input->name))||
       !isValidText(input->name)){
        printf("Invalid name\n");
        return 0;
    }

    printf("Enter Designation: ");

    if(!readText(input->designation,sizeof(input->designation))||
       !isValidText(input->designation)){
        printf("Invalid designation\n");
        return 0;
    }

    if(ceo!=NULL){
        printf("Enter Reporting Manager ID: ");

        if(!readInteger(managerId)||*managerId<=0){
            printf("Invalid manager ID\n");
            return 0;
        }

        if(*managerId==input->id){
            printf("An employee cannot report to themselves\n");
            return 0;
        }
    }

    return 1;
}

void addReport(Employee *manager,Employee *employee){
    Employee *last;

    employee->manager=manager;
    employee->nextSibling=NULL;

    if(manager->firstReport==NULL){
        manager->firstReport=employee;
        return;
    }

    last=manager->firstReport;

    while(last->nextSibling!=NULL)
        last=last->nextSibling;

    last->nextSibling=employee;
}

Employee *addEmployee(Employee *ceo){
    Employee input={0};
    Employee *manager=NULL;
    Employee *newEmployee;
    int managerId=0;

    if(!readEmployeeDetails(ceo,&input,&managerId))
        return ceo;

    if(ceo!=NULL){
        manager=findEmployee(ceo,managerId);

        if(manager==NULL){
            printf("Reporting manager not found\n");
            return ceo;
        }
    }

    newEmployee=malloc(sizeof(Employee));

    if(newEmployee==NULL){
        printf("Memory allocation failed\n");
        return ceo;
    }

    *newEmployee=input;
    newEmployee->manager=NULL;
    newEmployee->firstReport=NULL;
    newEmployee->nextSibling=NULL;

    if(manager==NULL)
        ceo=newEmployee;
    else
        addReport(manager,newEmployee);

    printf("Employee added successfully\n");

    return ceo;
}

int countDirectReports(const Employee *employee){
    const Employee *report;
    int count=0;

    if(employee==NULL)
        return 0;

    for(report=employee->firstReport;report!=NULL;report=report->nextSibling)
        count++;

    return count;
}

Employee *chooseNewParent(const Employee *employee){
    Employee *sibling,*best=NULL;
    int bestCount=0,count;

    for(sibling=employee->manager->firstReport;
        sibling!=NULL;
        sibling=sibling->nextSibling){

        if(sibling!=employee){
            count=countDirectReports(sibling);

            if(best==NULL||count<bestCount){
                best=sibling;
                bestCount=count;
            }
        }
    }

    if(best==NULL)
        return employee->manager;

    return best;
}

void detachFromManager(Employee *employee){
    Employee *manager=employee->manager;
    Employee *previous;

    if(manager->firstReport==employee){
        manager->firstReport=employee->nextSibling;
    }
    else{
        previous=manager->firstReport;

        while(previous->nextSibling!=employee)
            previous=previous->nextSibling;

        previous->nextSibling=employee->nextSibling;
    }

    employee->manager=NULL;
    employee->nextSibling=NULL;
}

void transferReports(Employee *employee,Employee *newParent){
    Employee *report=employee->firstReport;
    Employee *next;

    while(report!=NULL){
        next=report->nextSibling;
        report->nextSibling=NULL;
        addReport(newParent,report);
        report=next;
    }

    employee->firstReport=NULL;
}

Employee *removeEmployee(Employee *ceo){
    Employee *employee,*newParent;
    int hadReports;

    employee=readExistingEmployee(ceo,"Enter Employee ID to remove: ");

    if(employee==NULL)
        return ceo;

    if(employee==ceo){
        printf("CEO cannot be removed\n");
        return ceo;
    }

    hadReports=(employee->firstReport!=NULL);
    newParent=chooseNewParent(employee);

    detachFromManager(employee);
    transferReports(employee,newParent);
    free(employee);

    printf("Employee removed successfully\n");

    if(hadReports)
        printf("Direct reports moved under %d - %s\n",
               newParent->id,newParent->name);

    return ceo;
}

int countReports(const Employee *employee){
    const Employee *report;
    int total=0;

    if(employee==NULL)
        return 0;

    for(report=employee->firstReport;report!=NULL;report=report->nextSibling)
        total=total+1+countReports(report);

    return total;
}

void displayEmployees(const Employee *employee){
    const Employee *report;

    if(employee==NULL)
        return;

    printf("%d - %s - %s\n",
           employee->id,employee->name,employee->designation);

    for(report=employee->firstReport;report!=NULL;report=report->nextSibling)
        displayEmployees(report);
}

void displayEmployeeHierarchy(Employee *ceo){
    Employee *employee;

    employee=readExistingEmployee(ceo,"Enter Employee ID: ");

    if(employee==NULL)
        return;

    displayEmployees(employee);

    printf("Total Employees Under %d: %d\n",
           employee->id,countReports(employee));
}

void displayCompleteHierarchy(const Employee *ceo){
    if(ceo==NULL){
        printf("No employees in the organization\n");
        return;
    }

    printf("===== ORGANIZATION HIERARCHY =====\n");
    displayEmployees(ceo);
    printf("Total Employees: %d\n",1+countReports(ceo));
}

void freeHierarchy(Employee *employee){
    Employee *report,*next;

    if(employee==NULL)
        return;

    report=employee->firstReport;

    while(report!=NULL){
        next=report->nextSibling;
        freeHierarchy(report);
        report=next;
    }

    free(employee);
}

void displayMenu(void){
    printf("\n===== EMPLOYEE HIERARCHY MANAGEMENT =====\n");
    printf("1. Add Employee\n");
    printf("2. Remove Employee\n");
    printf("3. Display Employee Hierarchy\n");
    printf("4. Display Complete Hierarchy\n");
    printf("5. Exit\n");
    printf("Enter your choice: ");
}

int main(void){
    Employee *ceo=NULL;
    int choice;

    do{
        displayMenu();

        if(!readInteger(&choice))
            choice=INVALID_CHOICE;

        switch(choice){
            case ADD_EMPLOYEE:
                ceo=addEmployee(ceo);
                break;

            case REMOVE_EMPLOYEE:
                ceo=removeEmployee(ceo);
                break;

            case SHOW_EMPLOYEE_HIERARCHY:
                displayEmployeeHierarchy(ceo);
                break;

            case SHOW_COMPLETE_HIERARCHY:
                displayCompleteHierarchy(ceo);
                break;

            case EXIT_PROGRAM:
                printf("Exiting program\n");
                break;

            default:
                printf("Invalid choice\n");
        }
    }while(choice!=EXIT_PROGRAM);

    freeHierarchy(ceo);

    return 0;
}
