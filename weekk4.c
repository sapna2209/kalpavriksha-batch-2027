#include <stdio.h>
#include <stdlib.h>

#define MIN_SIZE 2
#define MAX_SIZE 10
#define MAX_VALUE 255

int readMatrixSize(void);
void generateMatrix(int *matrix,int size);
void displayMatrix(int *matrix,int size);
void swapValues(int *first,int *second);
void transposeMatrix(int *matrix,int size);
void reverseRows(int *matrix,int size);
void rotateMatrix(int *matrix,int size);
void copyRow(int *source,int *destination,int size);
int averageAround(int *matrix,int size,int *previous,int *current,int row,int column);
int smoothMatrix(int *matrix,int size);

int main(void){
    int size;
    int *matrix;

    size=readMatrixSize();

    if(size==0){
        return 1;
    }

    matrix=(int *)malloc(size*size*sizeof(int));

    if(matrix==NULL){
        printf("Memory allocation failed for matrix.\n");
        return 1;
    }

    generateMatrix(matrix,size);

    printf("\nOriginal Randomly Generated Matrix:\n");
    displayMatrix(matrix,size);

    rotateMatrix(matrix,size);

    printf("\nMatrix after 90 degree Clockwise Rotation:\n");
    displayMatrix(matrix,size);

    if(!smoothMatrix(matrix,size)){
        printf("Memory allocation failed for smoothing.\n");
        free(matrix);
        return 1;
    }

    printf("\nMatrix after Applying 3x3 Smoothing Filter:\n");
    displayMatrix(matrix,size);

    free(matrix);

    return 0;
}

int readMatrixSize(void){
    int size,ch,digit,valid;
    char input[100];
    int i;

    while(1){
        printf("Enter matrix size (%d-%d): ",MIN_SIZE,MAX_SIZE);

        if(fgets(input,sizeof(input),stdin)==NULL){
            return 0;
        }

        size=0;
        valid=1;
        i=0;

        while(input[i]!='\0'&&input[i]!='\n'){
            if(input[i]>='0'&&input[i]<='9'){
                digit=input[i]-'0';

                if(size>MAX_SIZE){
                    valid=0;
                }
                else{
                    size=size*10+digit;
                }
            }
            else{
                valid=0;
            }

            i++;
        }

        if(valid&&i>0&&size>=MIN_SIZE&&size<=MAX_SIZE){
            return size;
        }

        if(input[i]=='\0'&&i==99){
            while((ch=getchar())!='\n'&&ch!=EOF){
            }
        }

        printf("Invalid input. Enter a whole number between %d and %d.\n",MIN_SIZE,MAX_SIZE);
    }
}

/* We can use time.h with srand(time(NULL)) to generate a
   different pseudo-random sequence on each run, but it is
   not included here to keep the program simple. */

void generateMatrix(int *matrix,int size){
    int *cell;

    for(cell=matrix;cell<matrix+size*size;cell++){
        *cell=rand()%(MAX_VALUE+1);
    }
}

void displayMatrix(int *matrix,int size){
    int i,j;

    for(i=0;i<size;i++){
        for(j=0;j<size;j++){
            printf("%4d",*(matrix+i*size+j));
        }
        printf("\n");
    }
}

void swapValues(int *first,int *second){
    int temp=*first;
    *first=*second;
    *second=temp;
}

void transposeMatrix(int *matrix,int size){
    int i,j;

    for(i=0;i<size;i++){
        for(j=i+1;j<size;j++){
            swapValues(matrix+i*size+j,matrix+j*size+i);
        }
    }
}

void reverseRows(int *matrix,int size){
    int i,j;

    for(i=0;i<size;i++){
        for(j=0;j<size/2;j++){
            swapValues(matrix+i*size+j,matrix+i*size+(size-1-j));
        }
    }
}

void rotateMatrix(int *matrix,int size){
    transposeMatrix(matrix,size);
    reverseRows(matrix,size);
}

void copyRow(int *source,int *destination,int size){
    int i;

    for(i=0;i<size;i++){
        *(destination+i)=*(source+i);
    }
}

int averageAround(int *matrix,int size,int *previous,int *current,int row,int column){
    int sum=0,count=0,x,y;

    for(x=row-1;x<=row+1;x++){
        for(y=column-1;y<=column+1;y++){
            if(x>=0&&x<size&&y>=0&&y<size){
                if(x<row){
                    sum+=*(previous+y);
                }
                else if(x==row){
                    sum+=*(current+y);
                }
                else{
                    sum+=*(matrix+x*size+y);
                }
                count++;
            }
        }
    }

    return sum/count;
}

int smoothMatrix(int *matrix,int size){
    int *previous;
    int *current;
    int row,column;

    previous=(int *)malloc(size*sizeof(int));
    current=(int *)malloc(size*sizeof(int));

    if(previous==NULL||current==NULL){
        free(previous);
        free(current);
        return 0;
    }

    for(row=0;row<size;row++){
        copyRow(matrix+row*size,current,size);

        for(column=0;column<size;column++){
            *(matrix+row*size+column)=averageAround(matrix,size,previous,current,row,column);
        }

        copyRow(current,previous,size);
    }

    free(previous);
    free(current);

    return 1;
}
