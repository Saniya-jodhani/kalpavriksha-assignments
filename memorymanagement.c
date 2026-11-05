#include<stdio.h>
#include<string.h>
#include<limits.h>
#include <stdlib.h>

#define MIN_POOL_SIZE 1
#define MAX_POOL_SIZE 1000000

typedef struct {
    size_t start;
    size_t size;
    int allocationID;
    int isAllocated;
} MemoryBlock;

typedef enum {
    FIRST_FIT =1,
    BEST_FIT,
    WORST_FIT
} Strategy;

unsigned char *memoryPool = NULL;
MemoryBlock *blocks = NULL;

size_t poolSize = 0;
size_t blockCount = 0;
size_t capacity = 0;

long long nextID = 1;
Strategy currentStrategy = FIRST_FIT;

int readNumber(const char *message , long long *value){
    char input[100];
    char *end;

    while(1){
        printf("%s",message);

        if(fgets(input , sizeof(input),stdin)==NULL){
            return 0;
        }

        long long number = strtoll(input, &end, 10);

        if(end ==input){
            printf("Invalid input. Enter a valid number.\n");
            continue;
        }
        while(*end ==' '|| *end =='\t' || *end =='\n' || *end =='\r'){
            end++;
        }
        if(*end !='\0'){
            printf("Invalid input. Enter a valid number.\n");
            continue;
        }
        *value = number;
        return 1;
    }
}

int initializeMemory(size_t size){
    memoryPool = (unsigned char *) malloc(size);

    if(memoryPool == NULL){
        printf("Memory Pool allocation failed.\n");
        return 0;
    }
    capacity = 4;
    blocks = (MemoryBlock *)malloc(capacity * sizeof(MemoryBlock));

    if(blocks == NULL){
        printf("Metadata allocation failed.\n");
        free(memoryPool);
        memoryPool = NULL;
        return 0;
    }

    poolSize = size;
    blockCount = 0;
    blocks[0].start =0;
    blocks[0].size = size;
    blocks[0].allocationID =0;
    blocks[0].isAllocated =0;

    return 1;
}

int ensureCapacity(size_t required){
    if(required <= capacity){
        return 1;
    }
    size_t newCapacity = capacity;
    while(newCapacity < required){
        newCapacity *=2;
    }
    MemoryBlock *temp = (MemoryBlock *)realloc(blocks, newCapacity * sizeof(MemoryBlock));

    if(temp == NULL){
        printf("Unable to grow metadata array.\n");
        return 0;
    }

    blocks = temp;
    capacity = newCapacity;

    return 1;

}

int findSuitableBlock(size_t requiredSize){
    int selected = -1;


    for(size_t i=0; i < blockCount ; i++){
        if(blocks[i].isAllocated){
            continue;
        }

        if(blocks[i].size < requiredSize ){
        continue;
        } 

        if(currentStrategy == FIRST_FIT){
                return (int)i;
        }
        if(currentStrategy == BEST_FIT){
            if(selected == -1 || blocks[i].size < blocks[selected].size){
                selected = (int)i;
            }           
        }
        else if(currentStrategy == WORST_FIT){
            if(selected == -1 || blocks[i].size > blocks[selected].size){
                selected = (int)i;
            }
        }
    }
    return selected;
}

void checkFragmentation(size_t requestedSize){
    size_t totalFree = 0;
    printf("\n Free block sizes : ");
    for(size_t i=0; i < blockCount ;i++){
        if(!blocks[i].isAllocated){
            printf("%zu", blocks[i].size);
            totalFree += blocks[i].size;
        }
    }
    printf("bytes\n");

    if(totalFree < requestedSize){
        printf("External fragmentation detected!\n");
        printf("Requested : %zu bytes \n", requestedSize);

        printf("Total free memory : %zu bytes\n", totalFree);
        printf("Largest contiguous free block : ");
        size_t largest =0;
        for(size_t i=0; i < blockCount;i++){
            if(!blocks[i].isAllocated && blocks[i].size > largest){
                largest = blocks[i].size;
            }
        }
        printf("%zu bytes\n", largest);

    } else{
        printf("Insufficient total free memory.\n");
    }
}

void allocateMemory(size_t requestedSize){
    if(requestedSize ==0 || requestedSize > poolSize){
        printf("Invalid allocation size.\n");
        return ;
    }

    if(nextID > INT_MAX){
        printf("No more allocation IDs available.\n");
        return;
    }
    int index = findSuitableBlock(requestedSize);

    if(index == -1){
        printf("Allocation failed. no suitable free block\n");

        checkFragmentation(requestedSize);
        return;
    }
    MemoryBlock *block = &blocks[index];

    int needsSplit = block->size > requestedSize;

    if(needsSplit && !ensureCapacity(blockCount +1)){
        printf("Allocation failed.\n");
        return;
    }
    int id = (int)nextID++;

    if(needsSplit){
        memmove(&blocks[index+2],
            &blocks[index+1],
            (blockCount - index -1) * sizeof(MemoryBlock));

        blocks[index +1].start = block-> start + requestedSize;

        blocks[index+1].size = block->size - requestedSize;

        blocks[index+1].allocationID =0;
        blocks[index+1].isAllocated =0;
        
        blockCount++;
    }
    blocks[index].size = requestedSize;
    blocks[index].allocationID = id;
    blocks[index].isAllocated =1;

    printf("\n Allocation successful!\n");
    printf("Allocation ID : %d\n",id);
    printf("start offset : %zu\n",blocks[index].start);
    printf("Allocated size : %zu bytes\n",requestedSize);
}

int findAllocation(int id){
    for(size_t i=0; i < blockCount ; i++){
        if(blocks[i].isAllocated && blocks[i].allocationID==id){
            return (int )i;
        }
    }
    return -1;
}

void mergeFreeBlocks(){
    size_t i=0;
    while(i+1 < blockCount){
        if(!blocks[i].isAllocated && !blocks[i+1].isAllocated){
            blocks[i].size == blocks[i+1].size;
            memmove(&blocks[i+1],
                &blocks[i+2],
            (blockCount -i-2)*sizeof(MemoryBlock));
            blockCount--;
    
        } else{
            i++;
        }
    }
}

void releaseMemory(int id){
    int index = findAllocation(id);
    if(index ==-1){
        printf("Invalid allocation ID or block already freed.\n");
        return;
    }
    blocks[index].isAllocated =0;
    blocks[index].allocationID =0;

    printf("Allocation #%d released successfully.\n ",id);

    mergeFreeBlocks();
}

void resizeMemory(int id,size_t newSize ){
    int index = findAllocation(id);
    if(index == -1){
        printf("Invalid allocation ID.\n");
        return;
    }
    if(newSize ==0 || newSize > poolSize){
        printf("Invalid resize size.\n");
        return;
    }
    size_t oldSize = blocks[index].size;
    size_t oldStart = blocks[index].start;

    if(newSize == oldSize){
        printf("Allocation already has the required size.\n");
        return;
    }

    if(newSize < oldSize){
        if(!ensureCapacity(blockCount +1)){
            printf("Reize failed.\n");
            return;
        }
        memmove(&blocks[index+2],
            &blocks[index+1],
        (blockCount-index-1)*sizeof(MemoryBlock));

        blocks[index+1].start = oldStart + newSize;
        blocks[index+1].size = oldSize - newSize;
        blocks[index+1].allocationID =0;    
        blocks[index+1].isAllocated =0;

        blocks[index].size = newSize;
        blockCount++;
        mergeFreeBlocks();

        printf("Allocation #%d shrunk successfully .\n",id);
        return ;
    }

    size_t extraNeeded = newSize - oldSize;
    if(index +1 < blockCount && !blocks[index+1].isAllocated && blocks[index+1].size >= extraNeeded){
        if(blocks[index+1].size == extraNeeded){
            memmove(&blocks[index+1],
                &blocks[index+2],
                (blockCount - index -2)*sizeof(MemoryBlock));
            blockCount--;
        } else{
            blocks[index+1].start += extraNeeded;
            blocks[index+1].size -= extraNeeded;
        }
        blocks[index].size = newSize;
        printf("Allocation #%d expanded in place successfully.\n",id);
        return ;
    }

    int destination = findSuitableBlock(newSize);
    if(destination == -1){
        printf("Resize failed: no suitable free block.\n");
        checkFragmentation(newSize);
        printf("Original allocation remains unchanged.\n");
        return;
    }

    size_t destinationStart = blocks[destination].start;
    size_t destinationSize = blocks[destination].size;

    int needsSplit = destinationSize > newSize;
    if(needsSplit && !ensureCapacity(blockCount +1)){
        printf("Resize failed. Original allocation unchanged.\n");
        return ;
    }

    memmove( memoryPool + destinationStart,
        memoryPool + oldStart,
        oldSize);

    if(needsSplit){
        memmove(&blocks[destination+2],
            &blocks[destination+1],
            (blockCount - destination -1)* sizeof(MemoryBlock));
        
        blocks[destination +1].start = destinationStart + newSize;
        blocks[destination +1].size = destinationSize - newSize;

        blocks[destination +1].allocationID =0;
        blocks[destination +1].allocationID = 0;

        blockCount++;
    }
    blocks[destination].size = newSize;
    blocks[destination].allocationID = id;
    blocks[destination].isAllocated =1;

    int oldIndex = -1;

    for(size_t i=0; i < blockCount ; i++){
        if(blocks[i].start == oldStart && blocks[i].allocationID ==id && blocks[i].isAllocated){
            oldIndex = (int)i;
            break;
        }
    }
    if(oldIndex == -1){
        printf("Internal metadata error.\n");
        return;
    }
    blocks[oldIndex].isAllocated =0;
    blocks[oldIndex].allocationID =0;

    mergeFreeBlocks();

    printf("Allocation #%d moved successfully.\n",id);
    printf("Old start:%zu\n",oldStart);
    printf("New start:%zu\n",destinationStart);
    printf("New size:%zu\n",newSize);
}

void displayMemoryMap(){
    printf("\n========== MEMORY MAP ==========\n");
    printf("Total pool size: %zu bytes\n",poolSize);
    for(size_t i=0;  i < blockCount ;i++){
        if(blocks[i].isAllocated){
            printf("[Allocated #%d: %zu bytes, Start: %zu]\n", blocks[i].allocationID,blocks[i].size,blocks[i].start);
        } else{
            printf("[Free: %zu bytes, Start: %zu]\n", blocks[i].size,blocks[i].start);
        }
    }
    printf("================================\n");
}

void displayStatics(){
    size_t allocatedBytes =0;
    size_t freeBytes =0;

    size_t allocatedBlocks =0;
    size_t freeBlocks =0;

    size_t largestFreeBlock =0;

    for(size_t i=0; i < blockCount;i++){
        if(blocks[i].isAllocated){
            allocatedBytes += blocks[i].size;
            allocatedBlocks++;
        } else{
            freeBytes += blocks[i].size;
            freeBlocks++;
            if(blocks[i].size > largestFreeBlock){
                largestFreeBlock = blocks[i].size;
            }
        }
    }
    printf("\n==========MEMORY STATISTICS ==========\n");
    printf("Total pool size: %zu bytes\n", poolSize);
    printf("Allocated memory: %zu bytes\n", allocatedBytes);
    printf("Free memory: %zu bytes\n", freeBytes);
    printf("Allocated blocks: %zu\n", allocatedBlocks);
    printf("Free blocks: %zu\n", freeBlocks);
    printf("Largest free block: %zu bytes\n", largestFreeBlock);
    printf("================================\n");
}

void changeStrategy(){
    long long choice;
    printf("\nSelect allocation strategy:\n");
    printf("1. First Fit\n");
    printf("2. Best Fit\n");
    printf("3. Worst Fit\n");

    if(!readNumber("Enter choice: ", &choice)){
        return;
    }
    if(choice< FIRST_FIT || choice > WORST_FIT){
        printf("Invalid choice.\n");
        return;
    }
    if(currentStrategy == FIRST_FIT){
        printf("Strategy changed to First Fit.\n");
    } else if(currentStrategy == BEST_FIT){
        printf("Strategy changed to Best Fit.\n");
    } else {
        printf("Strategy changed to Worst Fit.\n");
    } 
}

int main(){
    long long input;
    
    while(1){
        if(!readNumber("Enter memory pool size(1-1000000):", &input)) return 0;

        if(input >=MIN_POOL_SIZE && input<=MAX_POOL_SIZE){
            break;
        }
        printf("Invalid pool size. Try again.\n");
    }
    if(!initializeMemory((size_t)input)) return 1;

    printf("\nMemory pool initialized successfully.\n");

    changeStrategy();

    while(1){
        printf("\nMemory Management Menu:\n");
        printf("1. Allocate Memory\n");
        printf("2. Release Memory\n");
        printf("3. Resize Memory\n");   
        printf("4. Display Memory Map\n");
        printf("5. Display Memory Statistics\n");
        printf("6. Change Allocation Strategy\n");
        printf("7. Exit\n");

        if(!readNumber("Enter choice:",&input)){
            break;
        }

        switch(input){
            case 1:{
                long long size;
                if(!readNumber("Enter requested size: ", &size)){
                    goto cleanup;
                }
                if(size<=0 || size>(long long)poolSize){
                    printf("Invalid allocation size.\n");
                    break;
                }
                allocateMemory((size_t)size);
                break;
            }
            case 2: {
                long long id;
                if(!readNumber("Enter allocation id: ", &id)){
                    goto cleanup;
                }
                if(id <=0 || id> INT_MAX){
                    printf("Invalid allocation id \n");
                    break;
                }
                releaseMemory((int)id);
                break;
            }
            case 3: {
                long long id;
                long long newSize;
                if(!readNumber("Enter allocation id: ", &id)){
                    goto cleanup;
                }
                if(id <=0 || id > INT_MAX){
                    printf("Invalid allocation Id. \n");
                    break;
                }
                if(!readNumber("Enter new Size: ", &newSize)){
                    goto cleanup;
                }
                if(newSize <=0 || newSize > (long long)poolSize){
                    printf("Invalid resize size.\n");
                    break;
                }
                resizeMemory((int)id, (size_t)newSize);
                break;
            }

            case 4:
                displayMemoryMap();
                break;

            case 5:
                displayStatics();
                break;

            case 6:
                changeStrategy();
                break;

            case 7:
                printf("Exiting smart memory management.\n");
                goto cleanup;

            default:
                printf("Invalid menu choice.\n");
        }
    }
    cleanup:
        free(memoryPool);
        free(blocks);

        memoryPool = NULL;
        blocks = NULL;

        printf("Memory pool and metadata released.\n");
        return 0;
}
   