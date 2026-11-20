#include <stdio.h>
#include <stdlib.h>

#define BLOCK_SIZE 512
#define NUM_BLOCKS 1024
#define MAX_NAME_LENGTH 50
#define MAX_INPUT_LENGTH 4096
#define COMMAND_COUNT 11

#if NUM_BLOCKS<1||NUM_BLOCKS>5000
#error NUM_BLOCKS must be between 1 and 5000
#endif

enum InputStatus{
    INPUT_EOF=0,
    INPUT_OK=1,
    INPUT_TOO_LONG=2
};

enum NodeType{
    FILE_TYPE=1,
    DIRECTORY_TYPE=2
};

enum Command{
    COMMAND_EXIT=1,
    COMMAND_LS=2,
    COMMAND_PWD=3,
    COMMAND_DF=4,
    COMMAND_MKDIR=5,
    COMMAND_CREATE=6,
    COMMAND_READ=7,
    COMMAND_DELETE=8,
    COMMAND_RMDIR=9,
    COMMAND_CD=10,
    COMMAND_WRITE=11
};

typedef struct FreeBlock FreeBlock;
typedef struct FileNode FileNode;
typedef struct VirtualFileSystem VirtualFileSystem;

struct FreeBlock{
    int index;
    FreeBlock *next;
    FreeBlock *prev;
};

struct FileNode{
    char name[MAX_NAME_LENGTH+1];
    int type;
    int size;
    int blockCount;
    int *blockPointers;
    FileNode *parent;
    FileNode *next;
    FileNode *prev;
    FileNode *children;
};

struct VirtualFileSystem{
    unsigned char (*virtualDisk)[BLOCK_SIZE];
    FreeBlock *freeHead;
    FreeBlock *freeTail;
    FreeBlock *blockNodes;
    int freeCount;
    FileNode *root;
    FileNode *cwd;
};

int readLine(char line[]);

FileNode *createNode(char name[],int type,FileNode *parent);
void insertChild(FileNode *directory,FileNode *node);
void removeChild(FileNode *directory,FileNode *node);
FileNode *findChild(FileNode *directory,char name[]);
void freeTree(FileNode *node);

void appendFreeBlock(VirtualFileSystem *vfs,int index);
int takeFreeBlock(VirtualFileSystem *vfs);
void releaseBlocks(VirtualFileSystem *vfs,FileNode *file);

int initializeVfs(VirtualFileSystem *vfs);
void destroyVfs(VirtualFileSystem *vfs);

void printPath(FileNode *directory);
void addEntry(VirtualFileSystem *vfs,char name[],int type);
void writeFile(VirtualFileSystem *vfs,char name[],char content[],int size);
void readFile(VirtualFileSystem *vfs,char name[]);
void deleteFile(VirtualFileSystem *vfs,char name[]);
void removeDirectory(VirtualFileSystem *vfs,char name[]);
void changeDirectory(VirtualFileSystem *vfs,char name[]);
void listDirectory(FileNode *directory);
void showDiskUsage(VirtualFileSystem *vfs);

void runWriteCommand(VirtualFileSystem *vfs,char name[],char line[],int position);
int executeCommand(VirtualFileSystem *vfs,char line[]);

int readLine(char line[]){
    char extra[2];
    int result;
    int tooLong=0;

    result=scanf("%4095[^\n]",line);

    if(result==EOF){
        return INPUT_EOF;
    }

    if(result==0){
        line[0]='\0';
    }

    result=scanf("%1[^\n]",extra);

    if(result==1){
        tooLong=1;
    }

    scanf("%*[^\n]");
    scanf("%*1[\n]");
    result=0;

    while(line[result]!='\0'){
        result++;
    }

    if(result>0&&line[result-1]=='\r'){
        line[result-1]='\0';
    }
    if(tooLong){
        return INPUT_TOO_LONG;
    }

    return INPUT_OK;
}

FileNode *createNode(char name[],int type,FileNode *parent){
    FileNode *node=malloc(sizeof(FileNode));
    int i=0;

    if(node==NULL){
        return NULL;
    }

    while(name[i]!='\0'){
        node->name[i]=name[i];
        i++;
    }

    node->name[i]='\0';
    node->type=type;
    node->size=0;
    node->blockCount=0;
    node->blockPointers=NULL;
    node->parent=parent;
    node->next=NULL;
    node->prev=NULL;
    node->children=NULL;

    return node;
}

void insertChild(FileNode *directory,FileNode *node){
    FileNode *tail;

    if(directory->children==NULL){
        node->next=node;
        node->prev=node;
        directory->children=node;
        return;
    }

    tail=directory->children->prev;
    tail->next=node;
    node->prev=tail;
    node->next=directory->children;
    directory->children->prev=node;
}

void removeChild(FileNode *directory,FileNode *node){

    if(node->next==node){
        directory->children=NULL;
    }
    else{
        node->prev->next=node->next;
        node->next->prev=node->prev;

        if(directory->children==node){
            directory->children=node->next;
        }
    }

    node->next=NULL;
    node->prev=NULL;
}

FileNode *findChild(FileNode *directory,char name[]){
    FileNode *current=directory->children;
    int i;

    if(current==NULL){
        return NULL;
    }

    do{
        i=0;

        while(current->name[i]!='\0'&&current->name[i]==name[i]){
            i++;
        }

        if(current->name[i]==name[i]){
            return current;
        }

        current=current->next;
    }while(current!=directory->children);

    return NULL;
}

void freeTree(FileNode *node){
    FileNode *first,*current,*next;

    if(node==NULL){
        return;
    }

    if(node->children!=NULL){
        first=node->children;
        current=first->next;

        while(current!=first){
            next=current->next;
            freeTree(current);
            current=next;
        }

        freeTree(first);
    }

    free(node->blockPointers);
    free(node);
}

void appendFreeBlock(VirtualFileSystem *vfs,int index){
    FreeBlock *node=&vfs->blockNodes[index];

    node->index=index;
    node->next=NULL;
    node->prev=vfs->freeTail;

    if(vfs->freeTail==NULL){
        vfs->freeHead=node;
    }
    else{
        vfs->freeTail->next=node;
    }

    vfs->freeTail=node;
    vfs->freeCount++;
}

int takeFreeBlock(VirtualFileSystem *vfs){
    FreeBlock *node=vfs->freeHead;
    int index;

    if(node==NULL){
        return -1;
    }

    vfs->freeHead=node->next;

    if(vfs->freeHead==NULL){
        vfs->freeTail=NULL;
    }
    else{
        vfs->freeHead->prev=NULL;
    }

    index=node->index;

    node->next=NULL;
    node->prev=NULL;

    vfs->freeCount--;

    return index;
}

void releaseBlocks(VirtualFileSystem *vfs,FileNode *file){
    int i;

    for(i=0;i<file->blockCount;i++){
        appendFreeBlock(vfs,file->blockPointers[i]);
    }

    free(file->blockPointers);

    file->blockPointers=NULL;
    file->blockCount=0;
    file->size=0;
}

int initializeVfs(VirtualFileSystem *vfs){
    int i;

    vfs->virtualDisk=malloc(NUM_BLOCKS*sizeof(*vfs->virtualDisk));

    if(vfs->virtualDisk==NULL){
        return 0;
    }

    vfs->blockNodes=malloc(NUM_BLOCKS*sizeof(FreeBlock));

    if(vfs->blockNodes==NULL){
        destroyVfs(vfs);
        return 0;
    }

    for(i=0;i<NUM_BLOCKS;i++){
        appendFreeBlock(vfs,i);
    }

    vfs->root=createNode("/",DIRECTORY_TYPE,NULL);

    if(vfs->root==NULL){
        destroyVfs(vfs);
        return 0;
    }

    vfs->cwd=vfs->root;

    return 1;
}

void destroyVfs(VirtualFileSystem *vfs){
    freeTree(vfs->root);

    free(vfs->blockNodes);
    free(vfs->virtualDisk);

    vfs->freeHead=NULL;
    vfs->freeTail=NULL;
    vfs->blockNodes=NULL;
    vfs->freeCount=0;
    vfs->root=NULL;
    vfs->cwd=NULL;
    vfs->virtualDisk=NULL;
}



void printPath(FileNode *directory){

    if(directory->parent==NULL){
        printf("/");
        return;
    }

    if(directory->parent->parent!=NULL){
        printPath(directory->parent);
    }

    printf("/%s",directory->name);
}

void addEntry(VirtualFileSystem *vfs,char name[],int type){
    FileNode *node;
    char *label=(type==DIRECTORY_TYPE)?"Directory":"File";
    int length=0,hasSlash=0;

    while(name[length]!='\0'){
        if(name[length]=='/'){
            hasSlash=1;
        }
        length++;
    }

    if(length==0||length>MAX_NAME_LENGTH){
        printf("Name must be 1 to %d characters long.\n",MAX_NAME_LENGTH);
        return;
    }

    if(hasSlash){
        printf("Name cannot contain '/'.\n");
        return;
    }

    if((name[0]=='.'&&name[1]=='\0')||(name[0]=='.'&&name[1]=='.'&&name[2]=='\0')){
        printf("'%s' is a reserved name.\n",name);
        return;
    }

    if(findChild(vfs->cwd,name)!=NULL){
        printf("'%s' already exists in this directory.\n",name);
        return;
    }

    node=createNode(name,type,vfs->cwd);

    if(node==NULL){
        printf("Memory allocation failed.\n");
        return;
    }

    insertChild(vfs->cwd,node);
    printf("%s '%s' created successfully.\n",label,name);
}


void writeFile(VirtualFileSystem *vfs,char name[],char content[],int size){
    FileNode *file=findChild(vfs->cwd,name);
    int needed=(size+BLOCK_SIZE-1)/BLOCK_SIZE;
    int *newPointers=NULL;
    int i,j,block,offset=0;

    if(file==NULL){
        printf("File '%s' not found.\n",name);
        return;
    }

    if(file->type==DIRECTORY_TYPE){
        printf("'%s' is a directory, not a file.\n",name);
        return;
    }

    if(needed>vfs->freeCount+file->blockCount){
        printf("Disk full: %d blocks needed, only %d available. File not changed.\n",
               needed,vfs->freeCount+file->blockCount);
        return;
    }

    if(needed>0){
        newPointers=malloc(needed*sizeof(int));

        if(newPointers==NULL){
            printf("Memory allocation failed. File not changed.\n");
            return;
        }
    }

    for(i=0;i<needed;i++){
        block=takeFreeBlock(vfs);

        if(block<0){
            
            for(j=0;j<i;j++){
                appendFreeBlock(vfs,newPointers[j]);
            }

            free(newPointers);

            printf("Block allocation failed. File not changed.\n");
            return;
        }

        newPointers[i]=block;
    }
    for(i=0;i<needed;i++){
        j=0;

        while(j<BLOCK_SIZE && offset<size){
            vfs->virtualDisk[newPointers[i]][j]=(unsigned char)content[offset];
            j++;
            offset++;
        }
    }

    releaseBlocks(vfs,file);

    file->blockPointers=newPointers;
    file->blockCount=needed;
    file->size=size;

    printf("Data written successfully (size=%d bytes).\n",size);
}

void readFile(VirtualFileSystem *vfs,char name[]){
    FileNode *file=findChild(vfs->cwd,name);
    int i,j,remaining;

    if(file==NULL){
        printf("File '%s' not found.\n",name);
        return;
    }

    if(file->type==DIRECTORY_TYPE){
        printf("'%s' is a directory, not a file.\n",name);
        return;
    }

    if(file->size==0){
        printf("(empty file)\n");
        return;
    }

    remaining=file->size;

    for(i=0;i<file->blockCount;i++){
        j=0;

        while(j<BLOCK_SIZE&&remaining>0){
            printf("%c",vfs->virtualDisk[file->blockPointers[i]][j]);
            j++;
            remaining--;
        }
    }

    printf("\n");
}

void deleteFile(VirtualFileSystem *vfs,char name[]){
    FileNode *file=findChild(vfs->cwd,name);

    if(file==NULL){
        printf("File '%s' not found.\n",name);
        return;
    }

    if(file->type==DIRECTORY_TYPE){
        printf("'%s' is a directory. Use rmdir to remove it.\n",name);
        return;
    }

    releaseBlocks(vfs,file);
    removeChild(vfs->cwd,file);
    free(file);

    printf("File deleted successfully.\n");
}

void removeDirectory(VirtualFileSystem *vfs,char name[]){
    FileNode *directory=findChild(vfs->cwd,name);

    if(directory==NULL){
        printf("Directory '%s' not found.\n",name);
        return;
    }

    if(directory->type==FILE_TYPE){
        printf("'%s' is a file. Use delete to remove it.\n",name);
        return;
    }

    if(directory->children!=NULL){
        printf("Directory '%s' is not empty.\n",name);
        return;
    }

    removeChild(vfs->cwd,directory);
    free(directory);

    printf("Directory removed successfully.\n");
}

void changeDirectory(VirtualFileSystem *vfs,char name[]){
    FileNode *target;

    if(name[0]=='/'&&name[1]=='\0'){
        target=vfs->root;
    }
    else if(name[0]=='.'&&name[1]=='.'&&name[2]=='\0'){
        target=(vfs->cwd->parent==NULL)?vfs->cwd:vfs->cwd->parent;
    }
    else if(name[0]=='.'&&name[1]=='\0'){
        target=vfs->cwd;
    }
    else{
        target=findChild(vfs->cwd,name);

        if(target==NULL){
            printf("Directory '%s' not found.\n",name);
            return;
        }

        if(target->type==FILE_TYPE){
            printf("'%s' is a file, not a directory.\n",name);
            return;
        }
    }

    vfs->cwd=target;
    printf("Moved to ");
    printPath(vfs->cwd);
    printf("\n");
}

void listDirectory(FileNode *directory){
    FileNode *current=directory->children;

    if(current==NULL){
        printf("(empty)\n");
        return;
    }

    do{
        if(current->type==DIRECTORY_TYPE){
            printf("%s/\n",current->name);
        }
        else{
            printf("%s\n",current->name);
        }
        current=current->next;
    }while(current!=directory->children);
}

void showDiskUsage(VirtualFileSystem *vfs){
    int used=NUM_BLOCKS-vfs->freeCount;

    printf("Total Blocks: %d\n",NUM_BLOCKS);
    printf("Used Blocks: %d\n",used);
    printf("Free Blocks: %d\n",vfs->freeCount);
    printf("Disk Usage: %.2f%%\n",(used*100.0)/NUM_BLOCKS);
}

void runWriteCommand(VirtualFileSystem *vfs,char name[],char line[],int position){
    char quote=line[position];
    char content[MAX_INPUT_LENGTH];
    int end=position;
    int i=position+1;
    int j=0;

    if(quote!='"'&&quote!='\''){
        printf("Usage: write <name> \"content\"\n");
        return;
    }

    while(line[end]!='\0'){
        end++;
    }

    end--;

    while(end>position&&(line[end]==' '||line[end]=='\t')){
        end--;
    }

    if(end<=position||line[end]!=quote){
        printf("Content must start and end with the same quote.\n");
        return;
    }

    while(i<end){
        if(line[i]=='\\'&&i+1<end&&line[i+1]==quote){
            content[j++]=quote;
            i+=2;
        }
        else{
            content[j++]=line[i++];
        }
    }

    content[j]='\0';

    writeFile(vfs,name,content,j);
}

int executeCommand(VirtualFileSystem *vfs,char line[]){
    char command[MAX_INPUT_LENGTH];
    char name[MAX_INPUT_LENGTH];
    char *commandNames[COMMAND_COUNT]={"exit","ls","pwd","df","mkdir","create","read","delete","rmdir","cd","write"};
    int argumentsNeeded[COMMAND_COUNT]={0,0,0,0,1,1,1,1,1,1,2};
    int position=0,commandLength=0,nameLength=0,code=0,i,k;

    while(line[position]==' '||line[position]=='\t'){
        position++;
    }

    while(line[position]!='\0'&&line[position]!=' '&&line[position]!='\t'){
        if(commandLength>=MAX_INPUT_LENGTH-1){
            printf("Command is too long.\n");
            return 1;
        }

        command[commandLength++]=line[position++];
    }

    command[commandLength]='\0';

    if(commandLength==0){
        return 1;
    }

    while(line[position]==' '||line[position]=='\t'){
        position++;
    }

    while(line[position]!='\0'&&line[position]!=' '&&line[position]!='\t'){
        if(nameLength>=MAX_INPUT_LENGTH-1){
            printf("Name or argument is too long.\n");
            return 1;
        }

        name[nameLength++]=line[position++];
    }

    name[nameLength]='\0';

    while(line[position]==' '||line[position]=='\t'){
        position++;
    }

    for(k=0;k<COMMAND_COUNT&&code==0;k++){
        i=0;

        while(command[i]!='\0'&&command[i]==commandNames[k][i]){
            i++;
        }

        if(command[i]==commandNames[k][i]){
            code=k+1;
        }
    }

    if(code==0){
        printf("Unknown command: %s\n",command);
        return 1;
    }

    if(argumentsNeeded[code-1]==0&&nameLength>0){
        printf("'%s' does not take any arguments.\n",command);
        return 1;
    }

    if(argumentsNeeded[code-1]==1&&nameLength==0){
        printf("Usage: %s <name>\n",command);
        return 1;
    }

    if(argumentsNeeded[code-1]==1&&line[position]!='\0'){
        printf("Too many arguments. Usage: %s <name>\n",command);
        return 1;
    }

    if(argumentsNeeded[code-1]==2&&nameLength==0){
        printf("Usage: write <name> \"content\"\n");
        return 1;
    }

    switch(code){
        case COMMAND_EXIT:
            return 0;
        case COMMAND_LS:
            listDirectory(vfs->cwd);
            break;
        case COMMAND_PWD:
            printPath(vfs->cwd);
            printf("\n");
            break;
        case COMMAND_DF:
            showDiskUsage(vfs);
            break;
        case COMMAND_MKDIR:
            addEntry(vfs,name,DIRECTORY_TYPE);
            break;
        case COMMAND_CREATE:
            addEntry(vfs,name,FILE_TYPE);
            break;
        case COMMAND_READ:
            readFile(vfs,name);
            break;
        case COMMAND_DELETE:
            deleteFile(vfs,name);
            break;
        case COMMAND_RMDIR:
            removeDirectory(vfs,name);
            break;
        case COMMAND_CD:
            changeDirectory(vfs,name);
            break;
        case COMMAND_WRITE:
            runWriteCommand(vfs,name,line,position);
            break;
    }

    return 1;
}

int main(void){
    VirtualFileSystem vfs={0};
    char line[MAX_INPUT_LENGTH];
    int status,running=1;

    if(!initializeVfs(&vfs)){
        printf("Virtual disk initialization failed.\n");
        return 1;
    }

    printf("Compact VFS - ready. Type 'exit' to quit.\n");

    while(running){
        printPath(vfs.cwd);
        printf(" > ");
        status=readLine(line);

        if(status==INPUT_EOF){
            printf("\n");
            running=0;
        }
        else if(status==INPUT_TOO_LONG){
            printf("Input too long (maximum %d characters).\n",MAX_INPUT_LENGTH-1);
        }
        else{
            running=executeCommand(&vfs,line);
        }
    }

    destroyVfs(&vfs);
    printf("Memory released. Exiting program...\n");

    return 0;
}
