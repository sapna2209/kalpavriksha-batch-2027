#include <stdio.h>
#include <ctype.h>

int main(){
    char str[1000];
    int stack[1000];
    int top=-1;
    int num=0;
    int sign=1;
    int hasNum=0;
    int numEnded=0;
    int expectNum=1;
    char op='+';

    fgets(str,sizeof(str),stdin);

    for(int i=0;str[i]!='\0'&&str[i]!='\n';i++){
        if(isdigit((unsigned char)str[i])){
            if(numEnded){
                printf("Error: Invalid expression.\n");
                return 0;
            }

            num=num*10+(str[i]-'0');
            hasNum=1;
            expectNum=0;
        }
        else if(str[i]==' '){
            if(hasNum){
                numEnded=1;
            }
        }
        else if(str[i]=='+'||str[i]=='-'||
                str[i]=='*'||str[i]=='/'){
            if(expectNum&&(str[i]=='+'||str[i]=='-')){
                if(numEnded){
                    printf("Error: Invalid expression.\n");
                    return 0;
                }

                if(str[i]=='-'){
                    sign=-sign;
                }
                continue;
            }

            if(expectNum||!hasNum){
                printf("Error: Invalid expression.\n");
                return 0;
            }

            num*=sign;

            if(op=='+'){
                stack[++top]=num;
            }
            else if(op=='-'){
                stack[++top]=-num;
            }
            else if(op=='*'){
                stack[top]*=num;
            }
            else if(op=='/'){
                if(num==0){
                    printf("Error: Division by zero.\n");
                    return 0;
                }

                stack[top]/=num;
            }

            num=0;
            sign=1;
            op=str[i];
            hasNum=0;
            numEnded=0;
            expectNum=1;
        }
        else{
            printf("Error: Invalid expression.\n");
            return 0;
        }
    }

    if(expectNum||!hasNum){
        printf("Error: Invalid expression.\n");
        return 0;
    }

    num*=sign;

    if(op=='+'){
        stack[++top]=num;
    }
    else if(op=='-'){
        stack[++top]=-num;
    }
    else if(op=='*'){
        stack[top]*=num;
    }
    else if(op=='/'){
        if(num==0){
            printf("Error: Division by zero.\n");
            return 0;
        }

        stack[top]/=num;
    }

    int ans=0;

    while(top>=0){
        ans+=stack[top--];
    }

    printf("%d\n",ans);

    return 0;
}
