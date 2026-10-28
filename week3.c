#include <stdio.h>
#include <limits.h>

#define MAX_INPUT_LENGTH 50
#define MAX_EMPLOYEES 100

typedef struct Employee Employee;

struct Employee{
    int id;
    char name[MAX_INPUT_LENGTH];
    char designation[MAX_INPUT_LENGTH];
    Employee *manager;
    Employee *reports[MAX_EMPLOYEES];
    int reportCount;
    int active;
};

int readInteger(int *value);
int readText(char text[],int size);
int isValidText(const char text[]);
Employee *findEmployee(Employee employees[],int id);
Employee *readExistingEmployee(Employee employees[],const char prompt[]);
int readEmployeeDetails(Employee employees[],Employee *input,int *managerId);
void addReport(Employee *manager,Employee *employee);
Employee *addEmployee(Employee employees[],Employee *ceo);
int countDirectReports(const Employee *employee);
Employee *chooseNewParent(const Employee *employee);
void detachFromManager(Employee *employee);
void transferReports(Employee *employee,Employee *newParent);
Employee *removeEmployee(Employee employees[],Employee *ceo);
int countReports(const Employee *employee);
void displayEmployees(const Employee *employee);
void displayEmployeeHierarchy(Employee employees[]);
void displayCompleteHierarchy(const Employee *ceo);
void displayMenu(void);

int readInteger(int *value){
    int ch,number=0,hasDigit=0,invalid=0;

    while((ch=getchar())!='\n'&&ch!=EOF){
        if(ch>='0'&&ch<='9'){
            int digit=ch-'0';
            hasDigit=1;

            if(number>(INT_MAX-digit)/10)
                invalid=1;
            else if(!invalid)
                number=number*10+digit;
        }
        else{
            invalid=1;
        }
    }

    if(ch==EOF&&!hasDigit&&!invalid)
        return -1;

    if(!hasDigit||invalid)
        return 0;

    *value=number;
    return 1;
}

int readText(char text[],int size){
    int ch,i=0,tooLong=0;

    while((ch=getchar())!='\n'&&ch!=EOF){
        if(i<size-1)
            text[i++]=(char)ch;
        else
            tooLong=1;
    }

    if(ch==EOF&&i==0&&!tooLong)
        return -1;

    text[i]='\0';

    if(tooLong)
        return 0;

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

Employee *findEmployee(Employee employees[],int id){
    int i;

    for(i=0;i<MAX_EMPLOYEES;i++){
        if(employees[i].active&&employees[i].id==id)
            return &employees[i];
    }

    return NULL;
}

Employee *readExistingEmployee(Employee employees[],const char prompt[]){
    Employee *employee;
    int id,result;

    printf("%s",prompt);
    result=readInteger(&id);

    if(result!=1||id<=0){
        printf("Invalid employee ID\n");
        return NULL;
    }

    employee=findEmployee(employees,id);

    if(employee==NULL)
        printf("Employee not found\n");

    return employee;
}

int readEmployeeDetails(Employee employees[],Employee *input,int *managerId){
    int result,i,hasEmployees=0;

    for(i=0;i<MAX_EMPLOYEES;i++){
        if(employees[i].active){
            hasEmployees=1;
            break;
        }
    }

    printf("Enter Employee ID: ");
    result=readInteger(&input->id);

    if(result!=1||input->id<1){
        printf("Invalid employee ID\n");
        return 0;
    }

    if(findEmployee(employees,input->id)!=NULL){
        printf("Employee ID already exists\n");
        return 0;
    }

    printf("Enter Name: ");
    result=readText(input->name,sizeof(input->name));

    if(result!=1||!isValidText(input->name)){
        printf("Invalid name\n");
        return 0;
    }

    printf("Enter Designation: ");
    result=readText(input->designation,sizeof(input->designation));

    if(result!=1||!isValidText(input->designation)){
        printf("Invalid designation\n");
        return 0;
    }

    if(hasEmployees){
        printf("Enter Reporting Manager ID: ");
        result=readInteger(managerId);

        if(result!=1||*managerId<=0){
            printf("Invalid manager ID\n");
            return 0;
        }

        if(*managerId==input->id){
            printf("An employee cannot report to themselves\n");
            return 0;
        }
    }
    else{
        *managerId=0;
        printf("This employee will be the CEO.\n");
    }

    return 1;
}

void addReport(Employee *manager,Employee *employee){
    if(manager==NULL||employee==NULL||manager->reportCount>=MAX_EMPLOYEES)
        return;

    employee->manager=manager;
    manager->reports[manager->reportCount]=employee;
    manager->reportCount++;
}

Employee *addEmployee(Employee employees[],Employee *ceo){
    Employee input={0};
    Employee *manager=NULL,*newEmployee=NULL;
    int managerId=0,i;

    if(!readEmployeeDetails(employees,&input,&managerId))
        return ceo;

    if(ceo!=NULL){
        manager=findEmployee(employees,managerId);

        if(manager==NULL){
            printf("Reporting manager not found\n");
            return ceo;
        }
    }

    for(i=0;i<MAX_EMPLOYEES;i++){
        if(!employees[i].active){
            newEmployee=&employees[i];
            break;
        }
    }

    if(newEmployee==NULL){
        printf("Employee limit reached\n");
        return ceo;
    }

    *newEmployee=input;
    newEmployee->manager=NULL;
    newEmployee->reportCount=0;
    newEmployee->active=1;

    if(manager==NULL)
        ceo=newEmployee;
    else
        addReport(manager,newEmployee);

    printf("Employee added successfully\n");
    return ceo;
}

int countDirectReports(const Employee *employee){
    if(employee==NULL)
        return 0;

    return employee->reportCount;
}

Employee *chooseNewParent(const Employee *employee){
    Employee *sibling,*best=NULL;
    int i,bestCount=0,count;

    for(i=0;i<employee->manager->reportCount;i++){
        sibling=employee->manager->reports[i];

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
    int i,j;

    if(manager==NULL)
        return;

    for(i=0;i<manager->reportCount;i++){
        if(manager->reports[i]==employee){
            for(j=i;j<manager->reportCount-1;j++)
                manager->reports[j]=manager->reports[j+1];

            manager->reportCount--;
            manager->reports[manager->reportCount]=NULL;
            break;
        }
    }

    employee->manager=NULL;
}

void transferReports(Employee *employee,Employee *newParent){
    int i;

    for(i=0;i<employee->reportCount;i++){
        Employee *report=employee->reports[i];
        addReport(newParent,report);
    }

    employee->reportCount=0;
}

Employee *removeEmployee(Employee employees[],Employee *ceo){
    Employee *employee,*newParent;
    int hadReports;

    employee=readExistingEmployee(employees,"Enter Employee ID to remove: ");

    if(employee==NULL)
        return ceo;

    if(employee==ceo){
        printf("CEO cannot be removed\n");
        return ceo;
    }

    hadReports=(employee->reportCount>0);
    newParent=chooseNewParent(employee);

    detachFromManager(employee);
    transferReports(employee,newParent);

    employee->active=0;
    employee->id=0;
    employee->name[0]='\0';
    employee->designation[0]='\0';
    employee->manager=NULL;

    printf("Employee removed successfully\n");

    if(hadReports)
        printf("Direct reports moved under %d - %s\n",newParent->id,newParent->name);

    return ceo;
}

int countReports(const Employee *employee){
    int i,total=0;

    if(employee==NULL)
        return 0;

    for(i=0;i<employee->reportCount;i++)
        total=total+1+countReports(employee->reports[i]);

    return total;
}

void displayEmployees(const Employee *employee){
    int i;

    if(employee==NULL)
        return;

    printf("%d - %s - %s\n",employee->id,employee->name,employee->designation);

    for(i=0;i<employee->reportCount;i++)
        displayEmployees(employee->reports[i]);
}

void displayEmployeeHierarchy(Employee employees[]){
    Employee *employee;

    employee=readExistingEmployee(employees,"Enter Employee ID: ");

    if(employee==NULL)
        return;

    displayEmployees(employee);
    printf("Total Employees Under %d: %d\n",employee->id,countReports(employee));
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
    Employee employees[MAX_EMPLOYEES]={0};
    Employee *ceo=NULL;
    int choice,result;

    while(1){
        displayMenu();
        result=readInteger(&choice);

        if(result==-1)
            break;

        if(result!=1)
            choice=0;

        switch(choice){
            case 1:
                ceo=addEmployee(employees,ceo);
                break;
            case 2:
                ceo=removeEmployee(employees,ceo);
                break;
            case 3:
                displayEmployeeHierarchy(employees);
                break;
            case 4:
                displayCompleteHierarchy(ceo);
                break;
            case 5:
                printf("Exiting program\n");
                return 0;
            default:
                printf("Invalid choice\n");
        }
    }

    return 0;
}
