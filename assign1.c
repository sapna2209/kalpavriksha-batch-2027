#include <stdio.h>
#include <ctype.h>

int main(){

    char str[100];
    int st[100];
    int top=-1;

    int num=0;
    char op='+';

    printf("Enter expression: ");
    fgets(str,sizeof(str),stdin);

    for(int i=0;str[i]!='\0'&&str[i]!='\n';i++){

        if(isdigit(str[i])){
            num=num*10+(str[i]-'0');
        }

        else if(str[i]==' '){
            continue;
        }

        else if(str[i]=='+'||str[i]=='-'||
                str[i]=='*'||str[i]=='/'){

            if(op=='+'){
                st[++top]=num;
            }
            else if(op=='-'){
                st[++top]=-num;
            }
            else if(op=='*'){
                st[top]=st[top]*num;
            }
            else if(op=='/'){

                if(num==0){
                    printf("Cannot divide by zero\n");
                    return 0;
                }

                st[top]=st[top]/num;
            }

            num=0;
            op=str[i];
        }

        else{
            printf("Invalid input\n");
            return 0;
        }
    }

    if(op=='+'){
        st[++top]=num;
    }
    else if(op=='-'){
        st[++top]=-num;
    }
    else if(op=='*'){
        st[top]=st[top]*num;
    }
    else if(op=='/'){

        if(num==0){
            printf("Cannot divide by zero\n");
            return 0;
        }

        st[top]=st[top]/num;
    }

    int ans=0;

    while(top>=0){
        ans=ans+st[top];
        top--;
    }

    printf("Answer: %d\n",ans);

    return 0;
}
