#include <stdio.h>
#include <stdlib.h>

#define MIN_SIZE 2
#define MAX_SIZE 10
#define MAX_VALUE 255

int readMatrixSize(void);
int ***generateMatrix(int size);
void displayMatrix(int size,int ***matrix);
void swapPointers(int **first,int **second);
void transposeMatrix(int size,int ***matrix);
void reverseRows(int size,int ***matrix);
void rotateMatrix(int size,int ***matrix);
void loadRow(int **cells,int *values,int size);
void copyRow(int *source,int *destination,int size);
int averageAround(int size,int ***matrix,int *previous,int *current,int row,int column);
void smoothMatrix(int size,int ***matrix);
void cleanUp(int size,int ***matrix);

int main(void){
    int size;
    int ***matrix;

    size=readMatrixSize();
    srand(1);

    matrix=generateMatrix(size);
    if(matrix==NULL){
        printf("Memory allocation failed\n");
        return 1;
    }

    printf("\nOriginal Randomly Generated Matrix:\n");
    displayMatrix(size,matrix);

    rotateMatrix(size,matrix);
    printf("\nMatrix after 90 degree Clockwise Rotation:\n");
    displayMatrix(size,matrix);

    smoothMatrix(size,matrix);
    printf("\nMatrix after Applying 3x3 Smoothing Filter:\n");
    displayMatrix(size,matrix);

    cleanUp(size,matrix);
    return 0;
}

int readMatrixSize(void){
    int size;

    while(1){
        printf("Enter matrix size (%d-%d): ",MIN_SIZE,MAX_SIZE);

        if(scanf("%d",&size)!=1){
            printf("Invalid input. Enter an integer.\n");
            exit(1);
        }

        if(size>=MIN_SIZE&&size<=MAX_SIZE){
            return size;
        }

        printf("Invalid size. Enter a number between %d and %d.\n",MIN_SIZE,MAX_SIZE);
    }
}

int ***generateMatrix(int size){
    int ***matrix;
    int i,j;

    matrix=calloc((size_t)size,sizeof(int **));
    if(matrix==NULL){
        return NULL;
    }

    for(i=0;i<size;i++){
        *(matrix+i)=calloc((size_t)size,sizeof(int *));
        if(*(matrix+i)==NULL){
            cleanUp(size,matrix);
            return NULL;
        }

        for(j=0;j<size;j++){
            *(*(matrix+i)+j)=malloc(sizeof(int));
            if(*(*(matrix+i)+j)==NULL){
                cleanUp(size,matrix);
                return NULL;
            }

            **(*(matrix+i)+j)=rand()%(MAX_VALUE+1);
        }
    }

    return matrix;
}

void displayMatrix(int size,int ***matrix){
    int i,j;

    for(i=0;i<size;i++){
        for(j=0;j<size;j++){
            printf("%4d",**(*(matrix+i)+j));
        }
        printf("\n");
    }
}

void swapPointers(int **first,int **second){
    int *temp=*first;
    *first=*second;
    *second=temp;
}

void transposeMatrix(int size,int ***matrix){
    int i,j;

    for(i=0;i<size;i++){
        for(j=i+1;j<size;j++){
            swapPointers(*(matrix+i)+j,*(matrix+j)+i);
        }
    }
}

void reverseRows(int size,int ***matrix){
    int i,j;

    for(i=0;i<size;i++){
        for(j=0;j<size/2;j++){
            swapPointers(*(matrix+i)+j,*(matrix+i)+(size-1-j));
        }
    }
}

void rotateMatrix(int size,int ***matrix){
    transposeMatrix(size,matrix);
    reverseRows(size,matrix);
}

void loadRow(int **cells,int *values,int size){
    int i;

    for(i=0;i<size;i++){
        *(values+i)=**(cells+i);
    }
}

void copyRow(int *source,int *destination,int size){
    int i;

    for(i=0;i<size;i++){
        *(destination+i)=*(source+i);
    }
}

int averageAround(int size,int ***matrix,int *previous,int *current,int row,int column){
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
                    sum+=**(*(matrix+x)+y);
                }

                count++;
            }
        }
    }

    return sum/count;
}

void smoothMatrix(int size,int ***matrix){
    int previous[MAX_SIZE]={0};
    int current[MAX_SIZE]={0};
    int row,column;

    for(row=0;row<size;row++){
        loadRow(*(matrix+row),current,size);

        for(column=0;column<size;column++){
            **(*(matrix+row)+column)=averageAround(size,matrix,previous,current,row,column);
        }

        copyRow(current,previous,size);
    }
}

void cleanUp(int size,int ***matrix){
    int i,j;

    for(i=0;i<size;i++){
        if(*(matrix+i)!=NULL){
            for(j=0;j<size;j++){
                free(*(*(matrix+i)+j));
            }

            free(*(matrix+i));
        }
    }

    free(matrix);
}
