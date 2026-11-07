#include <stdio.h>
#include <stdlib.h>

#define MIN_POOL 1
#define MAX_POOL 1000000
#define INITIAL_CAPACITY 4
#define MAX_ALLOCATION_ID 2147483647

enum Status{FREE=0,ALLOCATED=1};
enum Strategy{FIRST_FIT=1,BEST_FIT=2,WORST_FIT=3};
enum Menu{ALLOCATE=1,RELEASE=2,RESIZE=3,MAP=4,STATS=5,CHANGE=6,EXIT=7};


struct Block{
    int start;
    int size;
    int id;
    int status;
};

struct MemoryManager{
    unsigned char *memoryPool;
    struct Block *blockList;
    int blockCount;
    int blockCapacity;
    int poolSize;
    int nextAllocationId;
    int strategy;
    int pendingFragmentation;
    int failedRequestSize;
};

int readInteger(int minValue,int maxValue,const char *message);
void clearRange(struct MemoryManager *memory,int start,int size);
int initManager(struct MemoryManager *memory,int poolSize);
void cleanup(struct MemoryManager *memory);
void showMenu(void);
const char *strategyName(int strategy);
void chooseStrategy(struct MemoryManager *memory);
int insertBlock(struct MemoryManager *memory,int index,struct Block block);
void removeBlock(struct MemoryManager *memory,int index);
int firstFit(struct MemoryManager *memory,int size);
int bestFit(struct MemoryManager *memory,int size);
int worstFit(struct MemoryManager *memory,int size);
int findFreeBlock(struct MemoryManager *memory,int size);
int findBlockById(struct MemoryManager *memory,int id);
int findBlockByStart(struct MemoryManager *memory,int start);
int carveBlock(struct MemoryManager *memory,int index,int size,int id);
void mergeFreeBlocks(struct MemoryManager *memory);
int shrinkBlock(struct MemoryManager *memory,int index,int newSize);
int growBlock(struct MemoryManager *memory,int index,int newSize);
int totalFree(struct MemoryManager *memory);
void reportFragmentation(struct MemoryManager *memory,int size);
void updatePendingFragmentation(struct MemoryManager *memory);
void reportBadId(struct MemoryManager *memory,int id);
void allocateMemory(struct MemoryManager *memory);
void releaseMemory(struct MemoryManager *memory);
void resizeMemory(struct MemoryManager *memory);
void displayMap(struct MemoryManager *memory);
void displayStats(struct MemoryManager *memory);

int main(void){
    struct MemoryManager memory;
    int choice=0,poolSize;
    poolSize=readInteger(MIN_POOL,MAX_POOL,"Enter memory pool size: ");
    if(!initManager(&memory,poolSize)){
        printf("Memory allocation failed.\n");
        return 1;
    }
    chooseStrategy(&memory);
    showMenu();
    while(choice!=EXIT){
        printf("\n");
        choice=readInteger(ALLOCATE,EXIT,"Choice: ");
        switch(choice){
            case ALLOCATE:allocateMemory(&memory);break;
            case RELEASE:releaseMemory(&memory);break;
            case RESIZE:resizeMemory(&memory);break;
            case MAP:displayMap(&memory);break;
            case STATS:displayStats(&memory);break;
            case CHANGE:chooseStrategy(&memory);break;
            case EXIT:printf("Exiting.\n");break;
        }
    }
    cleanup(&memory);
    return 0;
}

int readInteger(int minValue,int maxValue,const char *message){
    int number,digits,valid,ch,started,ended,digit;
    while(1){
        number=0;digits=0;valid=1;started=0;ended=0;
        printf("%s",message);
        while((ch=getchar())!='\n'&&ch!=EOF){
            if(ch==' '||ch=='\t'||ch=='\r'){
                if(started){
                    ended=1;
                }
            }
            else if(ch>='0'&&ch<='9'){
                if(ended){
                    valid=0;
                }
                started=1;
                digits++;
                digit=ch-'0';
                if(number>(maxValue-digit)/10){
                    valid=0;
                }
                else if(valid){
                    number=number*10+digit;
                }
            }
            else{
                valid=0;
            }
        }
        if(ch==EOF&&digits==0){
            exit(0);
        }
        if(valid&&digits>0&&number>=minValue&&number<=maxValue){
            return number;
        }
        printf("Invalid input. Enter a number between %d and %d.\n",minValue,maxValue);
    }
}

int initManager(struct MemoryManager *memory,int poolSize){
    memory->memoryPool=NULL;
    memory->blockList=NULL;
    memory->memoryPool=malloc((size_t)poolSize*sizeof(unsigned char));
    memory->blockList=malloc(INITIAL_CAPACITY*sizeof(struct Block));
    if(memory->memoryPool==NULL||memory->blockList==NULL){
        free(memory->memoryPool);
        free(memory->blockList);
        memory->memoryPool=NULL;
        memory->blockList=NULL;
        return 0;
    }
    memory->blockList[0].start=0;
    memory->blockList[0].size=poolSize;
    memory->blockList[0].id=0;
    memory->blockList[0].status=FREE;
    memory->blockCount=1;
    memory->blockCapacity=INITIAL_CAPACITY;
    memory->poolSize=poolSize;
    memory->nextAllocationId=1;
    memory->strategy=FIRST_FIT;
    memory->pendingFragmentation=0;
    memory->failedRequestSize=0;
    return 1;
}

void cleanup(struct MemoryManager *memory){
    free(memory->memoryPool);
    free(memory->blockList);
    memory->memoryPool=NULL;
    memory->blockList=NULL;
}

void clearRange(struct MemoryManager *memory,int start,int size){
    int i;
    for(i=0;i<size;i++){
        memory->memoryPool[start+i]=0;
    }
}

void showMenu(void){
    printf("Menu:\n1. Allocate memory\n2. Release memory\n3. Resize allocation\n");
    printf("4. Display memory map\n5. Display memory statistics\n6. Change allocation strategy\n7. Exit\n");
}

const char *strategyName(int strategy){
    switch(strategy){
        case FIRST_FIT:return "First Fit";
        case BEST_FIT:return "Best Fit";
        default:return "Worst Fit";
    }
}

void chooseStrategy(struct MemoryManager *memory){
    memory->strategy=readInteger(FIRST_FIT,WORST_FIT,"Select allocation strategy (1. First Fit, 2. Best Fit, 3. Worst Fit): ");
    printf("Strategy set to %s.\n",strategyName(memory->strategy));
}

int insertBlock(struct MemoryManager *memory,int index,struct Block block){
    int i;
    struct Block *temp;
    if(memory->blockCount==memory->blockCapacity){
        temp=realloc(memory->blockList,(size_t)2*memory->blockCapacity*sizeof(struct Block));
        if(temp==NULL){
            return 0;
        }
        memory->blockList=temp;
        memory->blockCapacity*=2;
    }
    for(i=memory->blockCount;i>index;i--){
        memory->blockList[i]=memory->blockList[i-1];
    }
    memory->blockList[index]=block;
    memory->blockCount++;
    return 1;
}

void removeBlock(struct MemoryManager *memory,int index){
    int i;
    for(i=index;i<memory->blockCount-1;i++){
        memory->blockList[i]=memory->blockList[i+1];
    }
    memory->blockCount--;
}

int firstFit(struct MemoryManager *memory,int size){
    int i;
    for(i=0;i<memory->blockCount;i++){
        if(memory->blockList[i].status==FREE&&memory->blockList[i].size>=size){
            return i;
        }
    }
    return -1;
}

int bestFit(struct MemoryManager *memory,int size){
    int i,chosen=-1;
    for(i=0;i<memory->blockCount;i++){
        if(memory->blockList[i].status==FREE&&memory->blockList[i].size>=size){
            if(chosen==-1||memory->blockList[i].size<memory->blockList[chosen].size){
                chosen=i;
            }
        }
    }
    return chosen;
}

int worstFit(struct MemoryManager *memory,int size){
    int i,chosen=-1;
    for(i=0;i<memory->blockCount;i++){
        if(memory->blockList[i].status==FREE&&memory->blockList[i].size>=size){
            if(chosen==-1||memory->blockList[i].size>memory->blockList[chosen].size){
                chosen=i;
            }
        }
    }
    return chosen;
}

int findFreeBlock(struct MemoryManager *memory,int size){
    switch(memory->strategy){
        case FIRST_FIT:return firstFit(memory,size);
        case BEST_FIT:return bestFit(memory,size);
        default:return worstFit(memory,size);
    }
}

int findBlockById(struct MemoryManager *memory,int id){
    int i;
    for(i=0;i<memory->blockCount;i++){
        if(memory->blockList[i].status==ALLOCATED&&memory->blockList[i].id==id){
            return i;
        }
    }
    return -1;
}

int findBlockByStart(struct MemoryManager *memory,int start){
    int i;
    for(i=0;i<memory->blockCount;i++){
        if(memory->blockList[i].start==start){
            return i;
        }
    }
    return -1;
}

int carveBlock(struct MemoryManager *memory,int index,int size,int id){
    int oldSize=memory->blockList[index].size;
    struct Block rest;
    if(oldSize>size){
        rest.start=memory->blockList[index].start+size;
        rest.size=oldSize-size;
        rest.id=0;
        rest.status=FREE;
        if(!insertBlock(memory,index+1,rest)){
            return 0;
        }
    }
    memory->blockList[index].size=size;
    memory->blockList[index].id=id;
    memory->blockList[index].status=ALLOCATED;
    return 1;
}

void mergeFreeBlocks(struct MemoryManager *memory){
    int i=0;
    while(i<memory->blockCount-1){
        if(memory->blockList[i].status==FREE&&memory->blockList[i+1].status==FREE){
            memory->blockList[i].size+=memory->blockList[i+1].size;
            removeBlock(memory,i+1);
        }
        else{
            i++;
        }
    }
}

int shrinkBlock(struct MemoryManager *memory,int index,int newSize){
    int diff=memory->blockList[index].size-newSize;
    int next=index+1;
    struct Block freed;
    if(next<memory->blockCount&&memory->blockList[next].status==FREE){
        memory->blockList[next].start-=diff;
        memory->blockList[next].size+=diff;
    }
    else{
        freed.start=memory->blockList[index].start+newSize;
        freed.size=diff;
        freed.id=0;
        freed.status=FREE;
        if(!insertBlock(memory,next,freed)){
            return -1;
        }
    }
    clearRange(memory,memory->blockList[index].start+newSize,diff);
    memory->blockList[index].size=newSize;
    return 1;
}

int growBlock(struct MemoryManager *memory,int index,int newSize){
    int i,j,id,oldStart,oldSize,newStart,oldIndex;
    int diff=newSize-memory->blockList[index].size;
    int next=index+1;
    if(next<memory->blockCount&&memory->blockList[next].status==FREE&&memory->blockList[next].size>=diff){
        clearRange(memory,memory->blockList[index].start+memory->blockList[index].size,diff);
        memory->blockList[index].size=newSize;
        memory->blockList[next].start+=diff;
        memory->blockList[next].size-=diff;
        if(memory->blockList[next].size==0){
            removeBlock(memory,next);
        }
        return 1;
    }
    j=findFreeBlock(memory,newSize);
    if(j==-1){
        return 0;
    }
    id=memory->blockList[index].id;
    oldStart=memory->blockList[index].start;
    oldSize=memory->blockList[index].size;
    newStart=memory->blockList[j].start;
    if(!carveBlock(memory,j,newSize,id)){
        return -1;
    }
    for(i=0;i<oldSize;i++){
        memory->memoryPool[newStart+i]=memory->memoryPool[oldStart+i];
    }
    for(i=oldSize;i<newSize;i++){
        memory->memoryPool[newStart+i]=0;
    }
    oldIndex=findBlockByStart(memory,oldStart);
    clearRange(memory,oldStart,oldSize);
    memory->blockList[oldIndex].status=FREE;
    memory->blockList[oldIndex].id=0;
    mergeFreeBlocks(memory);
    return 1;
}

int totalFree(struct MemoryManager *memory){
    int i,total=0;
    for(i=0;i<memory->blockCount;i++){
        if(memory->blockList[i].status==FREE){
            total+=memory->blockList[i].size;
        }
    }
    return total;
}

void reportFragmentation(struct MemoryManager *memory,int size){
    memory->pendingFragmentation=0;
    memory->failedRequestSize=0;
    if(totalFree(memory)>=size){
        memory->pendingFragmentation=1;
        memory->failedRequestSize=size;
    }
}

void updatePendingFragmentation(struct MemoryManager *memory){
    if(memory->pendingFragmentation){
        if(totalFree(memory)<memory->failedRequestSize||
           findFreeBlock(memory,memory->failedRequestSize)!=-1){
            memory->pendingFragmentation=0;
            memory->failedRequestSize=0;
        }
    }
}

void reportBadId(struct MemoryManager *memory,int id){
    if(id<memory->nextAllocationId){
        printf("Invalid request: allocation %d is already free.\n",id);
    }
    else{
        printf("Invalid request: allocation ID %d does not exist.\n",id);
    }
}

void allocateMemory(struct MemoryManager *memory){
    int i,index,size,id=memory->nextAllocationId;
    size=readInteger(1,memory->poolSize,"Requested size: ");
    if(memory->nextAllocationId>=MAX_ALLOCATION_ID){
        printf("Allocation failed: allocation ID limit reached.\n");
        return;
    }
    index=findFreeBlock(memory,size);
    if(index==-1){
        printf("Allocation failed: no contiguous free block is large enough.\n");
        reportFragmentation(memory,size);
        return;
    }
    if(!carveBlock(memory,index,size,id)){
        printf("Allocation failed: could not grow the metadata array.\n");
        return;
    }
    for(i=0;i<size;i++){
        memory->memoryPool[memory->blockList[index].start+i]=0;
    }
    memory->nextAllocationId++;
    updatePendingFragmentation(memory);
    printf("Allocation successful. ID: %d, Start: %d, Size: %d\n",id,memory->blockList[index].start,size);
}

void releaseMemory(struct MemoryManager *memory){
    int index,id;
    if(memory->nextAllocationId==1){
        printf("There are no allocations to release.\n");
        return;
    }
    id=readInteger(1,memory->nextAllocationId-1,"Allocation ID: ");
    index=findBlockById(memory,id);
    if(index==-1){
        reportBadId(memory,id);
        return;
    }
    clearRange(memory,memory->blockList[index].start,memory->blockList[index].size);
    memory->blockList[index].status=FREE;
    memory->blockList[index].id=0;
    mergeFreeBlocks(memory);
    updatePendingFragmentation(memory);
    printf("Allocation released.\n");
}

void resizeMemory(struct MemoryManager *memory){
    int index,id,newSize,done;
    if(memory->nextAllocationId==1){
        printf("There are no allocations to resize.\n");
        return;
    }
    id=readInteger(1,memory->nextAllocationId-1,"Allocation ID: ");
    index=findBlockById(memory,id);
    if(index==-1){
        reportBadId(memory,id);
        return;
    }
    newSize=readInteger(1,memory->poolSize,"New size: ");
    if(newSize==memory->blockList[index].size){
        printf("Allocation already has that size.\n");
        return;
    }
    if(newSize<memory->blockList[index].size){
        done=shrinkBlock(memory,index,newSize);
    }
    else{
        done=growBlock(memory,index,newSize);
    }
    if(done==-1){
        printf("Resize failed: metadata memory allocation failed. Original allocation is unchanged.\n");
        return;
    }
    if(done==0){
        printf("Resize failed. Original allocation is unchanged.\n");
        return;
    }
    updatePendingFragmentation(memory);
    index=findBlockById(memory,id);
    printf("Resize successful. ID: %d, Start: %d, Size: %d\n",id,memory->blockList[index].start,memory->blockList[index].size);
}

void displayMap(struct MemoryManager *memory){
    int i;
    printf("Memory Map (%d bytes):\n",memory->poolSize);
    for(i=0;i<memory->blockCount;i++){
        if(memory->blockList[i].status==FREE){
            printf("[Free: %d bytes, Start: %d]\n",memory->blockList[i].size,memory->blockList[i].start);
        }
        else{
            printf("[Allocated #%d: %d bytes, Start: %d]\n",memory->blockList[i].id,memory->blockList[i].size,memory->blockList[i].start);
        }
    }
}

void displayStats(struct MemoryManager *memory){
    int i,freeBlocks=0,largest=0;
    int freeBytes=totalFree(memory);
    for(i=0;i<memory->blockCount;i++){
        if(memory->blockList[i].status==FREE){
            freeBlocks++;
            if(memory->blockList[i].size>largest){
                largest=memory->blockList[i].size;
            }
        }
    }
    printf("Total memory: %d bytes\n",memory->poolSize);
    printf("Allocated: %d bytes\n",memory->poolSize-freeBytes);
    printf("Free: %d bytes\n",freeBytes);
    printf("Free blocks: %d\n",freeBlocks);
    printf("Largest free block: %d bytes\n",largest);
    if(memory->pendingFragmentation){
        printf("Note: The request for %d bytes failed even though %d bytes are free, because the free memory is fragmented.\n",memory->failedRequestSize,freeBytes);
        memory->pendingFragmentation=0;
        memory->failedRequestSize=0;
    }
}
