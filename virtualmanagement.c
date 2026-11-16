#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define BLOCK_SIZE 512  //One disk block
#define NUM_BLOCKS 1024 //one virtual disk
#define MAX_NAME 50  //a filename can not be exceeded 50 characters
#define MAX_INPUT 4096 //max size of a command entered by the user
#define MAX_PATH 4096 //max buffer size used while handling directory paths

//structures

typedef enum {
    FILE_TYPE,
    DIRECTORY_TYPE
}NodeType;

typedef struct FileNode {
    char name[MAX_NAME + 1]; //name of file or directory
    NodeType type;  //tells whether it's a file or directory
    struct FileNode *parent; //stores the address of the directory in which teh current node exist
    struct FileNode *next;
    struct FileNode *prev;
    struct FileNode *children;
    int *blockPointers;
    int blockCount;
    size_t fileSize;
} FileNode;

typedef struct FreeBlock {
    int index;

    struct FreeBlock *next;
    struct FreeBlock *prev;
    
}FreeBlock;

unsigned char virtualDisk[NUM_BLOCKS][BLOCK_SIZE];

FreeBlock *freeHead = NULL;
FreeBlock *freetail = NULL;

FileNode *root = NULL;
FileNode *cwd = NULL;

int usedBlocks =0;
int freeBlocks = NUM_BLOCKS;

FileNode *findEntry(FileNode *directory , const char *name){
    if(directory ==NULL || directory->type != DIRECTORY_TYPE || directory->children == NULL){
        return NULL;
    }
    FileNode *temp = directory->children;
    do{
        if(strcmp(temp->name, name)==0){
            return temp;
        }
        temp = temp->next;
    }while(temp != directory->children);
    return NULL;
}

int validName(const char *name){
    if(name ==NULL ||strlen(name)==0 || strlen(name)>MAX_NAME){
        return 0;
    }
    if(strcmp(name ,".")==0 || strcmp(name,"..")==0)return 0;
    if(strchr(name,'/')!=NULL) return 0;
    return 1;
}

void insertChild(FileNode *directory, FileNode *newNode){

    if(directory == NULL || newNode == NULL) return;
    newNode->parent = directory;

    if(directory->children == NULL){
        newNode->next = newNode;
        newNode->prev = newNode;
        directory->children = newNode;
        return;
    }
    FileNode *head = directory->children;
    FileNode *tail = head->prev;

    tail->next = newNode;
    newNode->prev = tail;
    newNode->next = head;
    head->prev = newNode;
}

void removeChild(FileNode *directory, FileNode *node){
    if(directory == NULL || node == NULL)return;
    if(node->next == node){
        directory->children = NULL;
    }else{
        node->prev->next = node->next;
        node->next->prev = node->prev;

        if(directory->children == node){
            directory->children = node->next;
        }
    }
    node->next = NULL;
    node->prev = NULL;
}

void intializeFreeBlocks(){
    for(int i=0; i < NUM_BLOCKS;i++){
        FreeBlock *newBlock = (FreeBlock *)malloc(sizeof(FreeBlock));
        if(newBlock == NULL){
            printf("memory allocation failed.\n");
            exit(EXIT_FAILURE);
        }
        newBlock->index = i;
        newBlock->next = NULL;
        newBlock->prev = freetail;

        if(freeHead == NULL)freeHead = newBlock;
        else freetail->next = newBlock;

        freetail = newBlock;

    }
    freeBlocks = NUM_BLOCKS;
}

int allocateBlock(){
    if(freeHead == NULL) return -1;

    FreeBlock *temp = freeHead;
    int blockIndex = temp->index;
    freeHead = temp->next;
    if(freeHead == NULL) freetail= NULL;
    else freeHead->prev = NULL;

    free(temp);
    usedBlocks++;
    freeBlocks--;

    return blockIndex;
}

void releaseBlock(int index){
    if(index < 0 || index>=NUM_BLOCKS)return;
    FreeBlock *newBlock = (FreeBlock *)malloc(sizeof(FreeBlock));

    if(newBlock == NULL){
        printf("Warning: Unable to return block %d to free list.\n",
               index);
        return;
    }
    newBlock->index = index;
    newBlock->next = NULL;
    newBlock->prev = freetail;

    if(freetail == NULL) freeHead=newBlock;
    else freetail->next = newBlock;

    freetail = newBlock;
    usedBlocks--;
    freeBlocks++;

}

FileNode *createNode(const char *name, NodeType type){
    FileNode *newNode = (FileNode *)malloc(sizeof(FileNode));
    if(newNode== NULL){
        return NULL;
    }
    strcpy(newNode->name , name);
    newNode->type = type;
    newNode->parent= NULL;
    newNode->next = NULL;
    newNode->prev= NULL;
    newNode->children = NULL;

    newNode->blockPointers = NULL;
    newNode->blockCount = 0;
    newNode->fileSize = 0;
    return newNode;

}

void mkdirCommand(const char *name){
    if(!validName(name)){
        printf("Invalid directory name.\n");
        return;
    }
    if(findEntry(cwd,name) != NULL){
        printf("An entry with this name already exists.\n");
        return;
    }

    FileNode *newDirectory = createNode(name, DIRECTORY_TYPE);
    if(newDirectory == NULL){
        printf("Memory allocation failed.\n");
        return;
    }
    insertChild(cwd,newDirectory);
    printf("Directory '%s' created successfully.\n", name);
}

void createCommand(const char *name){
    if(!validName(name)){
        printf("Invalid file name.\n");
        return;
    }
    if(findEntry(cwd,name)!=NULL){
        printf("An entry with this name already exists.\n");
        return;
    }

    FileNode *newFile = createNode(name, FILE_TYPE);

    if(newFile ==NULL){
        printf("Memory allocation failed.\n");
        return;
    }
    insertChild(cwd,newFile);
    printf("File '%s' created successfully.\n", name);
}

FileNode *resolveDirectory(const char *path){
    if(path == NULL || strlen(path)==0)return cwd;

    char pathCopy[MAX_PATH];
    if(strlen(path)>= sizeof(pathCopy)) return NULL;

    strcpy(pathCopy,path);
    FileNode *current;
    if(pathCopy[0]=='/'){
        current = root;
    } else {
        current = cwd;
    }
    char *token = strtok(pathCopy,"/");
    while(token != NULL){
        if(strcmp(token,".")==0){

        }
        else if(strcmp(token,"..")==0) {
            if(current->parent != NULL){
                current = current->parent;

            }
        }
        else{
            FileNode *entry = findEntry(current,token);
            if(entry==NULL || entry->type != DIRECTORY_TYPE) return NULL;
            current = entry;
        }
        token = strtok(NULL,"/");
    }
    return current;
}

void cdCommand(const char *path){
    FileNode *target = resolveDirectory(path);
    if(target ==NULL){
        printf("Directory not found.\n");
        return;
    }
    cwd = target;
    printf("Moved to ");
    FileNode *stack[MAX_PATH/2];
    int count=0;
    FileNode *temp = cwd;

    while(temp!=NULL && temp->parent !=NULL && count < MAX_PATH/2){
        stack[count++] = temp;
        temp = temp->parent;
    }
    printf("/");

    for(int i=count-1;i >=0; i--){
        printf("%s",stack[i]->name);
        if(i!=0){
            printf("/");
        }
    }
    printf("\n");
}

void pwdCommand(){
    FileNode *stack[MAX_PATH/2];
    int count=0;
    FileNode *temp = cwd;
    while(temp!=NULL && temp->parent != NULL && count< MAX_PATH/2){
        stack[count++] = temp;
        temp = temp->parent;
    }
    printf("/");
    for(int i=count-1; i >=0;i--){
        printf("%s",stack[i]->name);
        if(i!=0){
            printf("/");
        }
    }
    printf("\n");
}

void lsCommand(const char *path){
    FileNode *directory;
    if(path ==NULL || strlen(path)==0) directory = cwd;
    else directory = resolveDirectory(path);

    if(directory ==NULL){
        printf("Directory not found.\n");
        return;
    }
    if(directory->children ==NULL){
        printf("(empty)\n");
        return;
    }
    FileNode *temp = directory->children;
    do{
        if(temp->type == DIRECTORY_TYPE){
            printf("[DIR]  %s/\n", temp->name);
        }else {
            printf("[FILE] %s (%zu bytes)\n",
                   temp->name, temp->fileSize);
        }
        temp = temp->next;
    }while(temp!= directory->children);
}

void writeCommand(const char *name , const char *content){
    FileNode *file = findEntry(cwd,name);
    if(file == NULL){
        printf("File not found. Create it first.\n");
        return;
    }
    if(file->type != FILE_TYPE){
        printf("Cannot write to a directory.\n");
        return;
    }
    size_t newSize = strlen(content);
    int requiredBlocks =(int)((newSize + BLOCK_SIZE-1)/BLOCK_SIZE);

    if(requiredBlocks>freeBlocks+ file->blockCount){
        printf("Disk is full. Not enough free blocks.\n");
        return;
    }
    int *newPointers = NULL;
    if(requiredBlocks >0){
        newPointers = (int *)malloc(requiredBlocks * sizeof(int));

        if(newPointers == NULL){
            printf("Memory allocation failed.\n");
            return;
        }
    }

    for(int i=0; i < file->blockCount; i++){
        releaseBlock(file->blockPointers[i]);
    }
    free(file->blockPointers);
    file-> blockPointers= NULL;
    file->blockCount = 0;
    file->fileSize =0;

    for(int i=0; i < requiredBlocks; i++){
        int index = allocateBlock();

        if(index==-1){
            printf("Block allocation failed.\n");
            for(int j=0;j < i;j++){
                releaseBlock(newPointers[j]);
            }
            free(newPointers);
            return;
        }
        newPointers[i] = index;
    }
    size_t offset =0;
    for(int i=0; i < requiredBlocks;i++){
        size_t remaining = newSize - offset;
        size_t bytesToCopy = remaining< BLOCK_SIZE ? remaining : BLOCK_SIZE;

        memcpy(virtualDisk[newPointers[i]],content+offset,bytesToCopy);

        offset+=bytesToCopy;
    }
    file->blockPointers = newPointers;
    file->blockCount = requiredBlocks;
    file->fileSize = newSize;

    printf("Data written successfully (size=%zu bytes).\n",
           newSize);
}

void readCommand(const char *name){
    FileNode *file = findEntry(cwd,name);
    if(file == NULL){
        printf("File not found.\n");
        return;
    }
    if(file->type != FILE_TYPE){
        printf("Cannot read a directory.\n");
        return;
    }

    if(file->fileSize ==0){
        printf("(empty file)\n");
        return;
    }
    size_t remaining = file->fileSize;
    for(int i=0; i < file->blockCount;i++){
        size_t bytesToRead = remaining < BLOCK_SIZE ? remaining : BLOCK_SIZE;

        fwrite(virtualDisk[file->blockPointers[i]],1, bytesToRead,stdout);

        remaining -= bytesToRead;
    }
    printf("\n");
}

void deleteCommand(const char *name){
    FileNode *file = findEntry(cwd,name);

    if(file == NULL){
        printf("File not found.\n");
        return;
    }
    if(file->type != FILE_TYPE){
        printf("This is a directory. Use rmdir instead.\n");
        return;
    }

    for(int i=0; i < file->blockCount;i++){
        releaseBlock(file->blockPointers[i]);
    }

    free(file->blockPointers);

    removeChild(cwd,file);
    free(file);
    printf("File deleted successfully.\n");
}

void rmdirCommand(const char *name){
    FileNode *directory = findEntry(cwd,name);
    if(directory == NULL){
        printf("Directory not found.\n");
        return;
    }
    if(directory->type !=  DIRECTORY_TYPE){
        printf("This is a file. Use delete instead.\n");
        return;
    }

    if(directory->children !=NULL){
        printf("Directory is not empty. Cannot remove it.\n");
        return;
    }
    removeChild(cwd, directory);
    free(directory);
    printf("Directory removed successfully.\n");
}

void dfCommand(){
    double usage = ((double)usedBlocks/NUM_BLOCKS) * 100.0;

    printf("\n");
    printf("Total Blocks: %d\n", NUM_BLOCKS);
    printf("Used Blocks: %d\n", usedBlocks);
    printf("Free Blocks: %d\n", freeBlocks);
    printf("Disk Usage: %.2f%%\n", usage);
    printf("\n");
}

void freeDirectoryTree(FileNode *directory){
    if(directory ==NULL) return;

    FileNode *child = directory->children;
    if(child!=NULL){
        FileNode *current = child;
        do{
            FileNode *next = current->next;
            if(current->type == DIRECTORY_TYPE){
                freeDirectoryTree(current);
            } else {
                free(current->blockPointers);
                current-> blockPointers = NULL;
            }
            free(current);
            current = next;
        } while(current!=child);
    }

    directory ->children = NULL;
}

void freeFreeBlockList(){
    FreeBlock *temp = freeHead;

    while(temp!=NULL){
        FreeBlock *next = temp->next;
        free(temp);
        temp = next;
    }

    freeHead = NULL;
    freetail = NULL;
}

void exitCommand(){
    freeDirectoryTree(root);
    free(root);
    root = NULL;
    cwd = NULL;
    freeFreeBlockList();
    printf("Memory released. Exiting program...\n");
}

void initializeVFS() {
    root = createNode("/", DIRECTORY_TYPE);

    if(root == NULL){
        printf("Unable to initialize root directory.\n");
        exit(EXIT_FAILURE);
    }

    root->parent = NULL;

    cwd = root;
    intializeFreeBlocks();
    printf("Compact VFS - ready. Type 'exit' to quit.\n");

}

void processCommand(char *input){
    while (isspace((unsigned char)*input)) {
        input++;
    }

    if (*input == '\0') {
        return;
    }

    // Find the end of the command without modifying input
    char *args = input;

    while (*args != '\0' && !isspace((unsigned char)*args)) {
        args++;
    }

    // Separate command from arguments
    if (*args != '\0') {
        *args = '\0';
        args++;

        while (isspace((unsigned char)*args)) {
            args++;
        }

        if (*args == '\0') {
            args = NULL;
        }
    } else {
        args = NULL;
    }

    char *command = input;

    if (strcmp(command, "exit") == 0) return;

    if (strcmp(command, "pwd") == 0) {
        pwdCommand();
        return;
    }

    if (strcmp(command, "df") == 0) {
        dfCommand();
        return;
    }

    if (strcmp(command, "ls") == 0) {
        lsCommand(args);
        return;
    }

    if (args == NULL) {
        printf("Missing command argument.\n");
        return;
    }
    
    if (strcmp(command, "mkdir") == 0) {

        char *name = strtok(args, " \t\r\n");

        if (name != NULL) {
            mkdirCommand(name);
        }

    }
    else if(strcmp(command,"create")==0){
        char *name = strtok(args," \t\r\n");

        if(name != NULL) createCommand(name);
    }

    else if(strcmp(command,"cd")==0){
        char *path = strtok(args," \t\r\n");
        if(path != NULL) cdCommand(path);
    }

    else if(strcmp(command,"delete")==0){
        char *name = strtok(args," \t\r\n");
        if(name != NULL)deleteCommand(name);
    }

    else if(strcmp(command,"rmdir")==0){
        char *name = strtok(args, " \t\r\n");
        if(name != NULL) rmdirCommand(name);
    }

    else if(strcmp(command, "write") == 0){

    char *name = args;
    char *content = name;

    while(*content != '\0' && !isspace((unsigned char)*content)){
        content++;
    }

    if(content == name){
        printf("Missing file name.\n");
        return;
    }
    if(*content != '\0'){
        *content = '\0';
        content++;

        while(isspace((unsigned char)*content)){
            content++;
        }
    }

    size_t len = strlen(content);

    if(len >= 2 && content[0] == '"' && content[len-1] == '"'){
        content[len-1] = '\0';
        content++;
    }

    writeCommand(name, content);
}

    else if(strcmp(command,"read")==0){
        char *name = strtok(args, " \t\r\n");

        if (name != NULL) {
            readCommand(name);
        }
    }
    else {
        printf("Unknown command: %s\n", command);
    }
}

int main()
{
    initializeVFS();
    char input[MAX_INPUT];

    while(1){
        pwdCommand();
        printf("> ");

        if(fgets(input, sizeof(input),stdin)==NULL){
            printf("\n");
            exitCommand();
            break;
        }
        char commandCopy[MAX_INPUT];
        strcpy(commandCopy,input);

        char *firstToken = strtok(commandCopy," \t\r\n");
        if(firstToken != NULL && strcmp(firstToken,"exit")==0){
            exitCommand();
            break;
        }
        processCommand(input);
    }
    return 0;
}