#include <stdio.h>
#include <stdlib.h>

#define blockSize 512
#define numberOfBlocks 1000
#define manBlocksForSingleFile 20

typedef struct freeBlockNode
{
    int index;
    struct freeBlockNode *prev;
    struct freeBlockNode *next;
} freeBlockNode;

typedef struct fileNode
{
    char name[50];
    int isDirectory;
    int blockPointers[manBlocksForSingleFile];
    int numBlocks;
    int size;
    struct fileNode *parent;
    struct fileNode *next;
    struct fileNode *prev;
    struct fileNode *children;
} fileNode;

char virtualDisk[numberOfBlocks][blockSize];
freeBlockNode *freeListHead = NULL;
freeBlockNode *freeListTail = NULL;

fileNode *root = NULL;
fileNode *cwd = NULL;

int str_len(const char *s)
{
    int i = 0;
    while (s[i] != '\0')
        i++;
    return i;
}

void str_copy(char *dest, const char *src)
{
    int i = 0;
    while (src[i] != '\0')
    {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

int str_cmp(const char *a, const char *b)
{
    int i = 0;
    while (a[i] != '\0' && b[i] != '\0')
    {
        if (a[i] != b[i])
            return 1;
        i++;
    }
    return (a[i] == b[i]) ? 0 : 1;
}

void readLine(char *buf, int max)
{
    char ch;
    int i = 0;
    while (i < max - 1)
    {
        if (scanf("%c", &ch) != 1)
            break;
        if (ch == '\n')
            break;
        buf[i++] = ch;
    }
    buf[i] = '\0';
}

void initializeFreeList()
{
    for (int i = 0; i < numberOfBlocks; i++)
    {
        freeBlockNode *block = (freeBlockNode *)malloc(sizeof(freeBlockNode));
        block->index = i;
        block->next = NULL;
        block->prev = freeListTail;

        if (freeListTail)
            freeListTail->next = block;
        else
            freeListHead = block;

        freeListTail = block;
    }
}

void initializeRoot()
{
    root = (fileNode *)malloc(sizeof(fileNode));
    str_copy(root->name, "/");
    root->isDirectory = 1;
    root->children = NULL;
    root->parent = NULL;
    root->next = root->prev = root;
    cwd = root;
}

fileNode *findNodeInDir(fileNode *dir, const char *name)
{
    if (!dir->children)
        return NULL;
    fileNode *temp = dir->children;
    do
    {
        if (str_cmp(temp->name, name) == 0)
            return temp;
        temp = temp->next;
    } while (temp != dir->children);
    return NULL;
}

void insertChild(fileNode *parent, fileNode *child)
{
    if (!parent->children)
    {
        parent->children = child;
        child->next = child->prev = child;
        return;
    }
    fileNode *last = parent->children->prev;
    last->next = child;
    child->prev = last;
    child->next = parent->children;
    parent->children->prev = child;
}

void removeChild(fileNode *parent, fileNode *target)
{
    if (!parent->children)
        return;
    fileNode *head = parent->children;
    if (head == target && head->next == head)
    {
        parent->children = NULL;
    }
    else
    {
        if (head == target)
            parent->children = head->next;
        target->prev->next = target->next;
        target->next->prev = target->prev;
    }
    free(target);
}

void allocateBlocks(fileNode *file, const char *data)
{
    int dataSize = str_len(data);
    if (dataSize == 0)
        return;

    int neededBlocks = (dataSize + blockSize - 1) / blockSize;
    file->numBlocks = 0;
    file->size = dataSize;

    for (int i = 0; i < neededBlocks && freeListHead != NULL; i++)
    {
        freeBlockNode *block = freeListHead;
        freeListHead = freeListHead->next;
        if (freeListHead)
            freeListHead->prev = NULL;
        file->blockPointers[file->numBlocks++] = block->index;

        int start = i * blockSize;
        for (int j = 0; j < blockSize && (start + j) < dataSize; j++)
            virtualDisk[block->index][j] = data[start + j];
        virtualDisk[block->index][dataSize % blockSize] = '\0';

        free(block);
    }
}

void freeBlocks(fileNode *file)
{
    for (int i = 0; i < file->numBlocks; i++)
    {
        freeBlockNode *block = (freeBlockNode *)malloc(sizeof(freeBlockNode));
        block->index = file->blockPointers[i];
        block->next = NULL;
        block->prev = freeListTail;
        if (freeListTail)
            freeListTail->next = block;
        else
            freeListHead = block;
        freeListTail = block;
    }
}

void mkdirCommand(const char *name)
{
    fileNode *newDir = (fileNode *)malloc(sizeof(fileNode));
    str_copy(newDir->name, name);
    newDir->isDirectory = 1;
    newDir->children = NULL;
    newDir->parent = cwd;
    newDir->next = newDir->prev = newDir;
    insertChild(cwd, newDir);
    printf("Directory '%s' created successfully.\n", name);
}

void createCommand(const char *name)
{
    fileNode *newFile = (fileNode *)malloc(sizeof(fileNode));
    str_copy(newFile->name, name);
    newFile->isDirectory = 0;
    newFile->numBlocks = 0;
    newFile->size = 0;
    newFile->parent = cwd;
    newFile->next = newFile->prev = newFile;
    insertChild(cwd, newFile);
    printf("File '%s' created successfully.\n", name);
}

void writeCommand(const char *filename, const char *data)
{
    fileNode *file = findNodeInDir(cwd, filename);
    if (!file || file->isDirectory)
    {
        printf("Error: File not found.\n");
        return;
    }
    allocateBlocks(file, data);
    printf("Data written successfully (size=%d bytes).\n", file->size);
}

void readCommand(const char *filename)
{
    fileNode *file = findNodeInDir(cwd, filename);
    if (!file || file->isDirectory)
    {
        printf("Error: File not found.\n");
        return;
    }
    for (int i = 0; i < file->numBlocks; i++)
        printf("%s", virtualDisk[file->blockPointers[i]]);
    printf("\n");
}

void deleteCommand(const char *filename)
{
    fileNode *file = findNodeInDir(cwd, filename);
    if (!file || file->isDirectory)
    {
        printf("Error: File not found.\n");
        return;
    }
    freeBlocks(file);
    removeChild(cwd, file);
    printf("File '%s' deleted successfully.\n", filename);
}

void rmdirCommand(const char *dirname)
{
    fileNode *dir = findNodeInDir(cwd, dirname);
    if (!dir || !dir->isDirectory)
    {
        printf("Error: Directory not found.\n");
        return;
    }
    if (dir->children != NULL)
    {
        printf("Error: Directory not empty.\n");
        return;
    }
    removeChild(cwd, dir);
    printf("Directory '%s' removed successfully.\n", dirname);
}

void lsCommand()
{
    if (cwd->children == NULL)
    {
        printf("(empty)\n");
        return;
    }
    fileNode *temp = cwd->children;
    do
    {
        printf("%s%s\n", temp->name, temp->isDirectory ? "/" : "");
        temp = temp->next;
    } while (temp != cwd->children);
}

void cdCommand(const char *dirname)
{
    if (str_cmp(dirname, "..") == 0)
    {
        if (cwd->parent != NULL)
            cwd = cwd->parent;
        printf("Moved to %s\n", cwd->name);
        return;
    }
    fileNode *dir = findNodeInDir(cwd, dirname);
    if (!dir || !dir->isDirectory)
    {
        printf("Error: Directory not found.\n");
        return;
    }
    cwd = dir;
    printf("Moved to /%s\n", cwd->name);
}

void pwdCommand(fileNode *dir)
{
    if (dir == NULL)
        return;
    if (dir->parent != NULL)
    {
        pwdCommand(dir->parent);
        if (str_cmp(dir->name, "/") != 0)
            printf("/%s", dir->name);
    }
    else
    {
        printf("/");
    }
}

void dfCommand()
{
    int freeCount = 0;
    for (freeBlockNode *tmp = freeListHead; tmp != NULL; tmp = tmp->next)
        freeCount++;
    int used = numberOfBlocks - freeCount;
    printf("Total Blocks: %d\nUsed Blocks: %d\nFree Blocks: %d\nDisk Usage: %.2f%%\n",
           numberOfBlocks, used, freeCount, (used * 100.0) / numberOfBlocks);
}

int main()
{
    initializeFreeList();
    initializeRoot();
    printf("Compact VFS - ready. Type 'exit' to quit.\n");

    char command[50], fileOrDirName[50], fileContent[512];
    int i;
    char ch;

    while (1)
    {
        printf("%s > ", cwd->name);

        for (i = 0; i < 50; i++)
        {
            command[i] = fileOrDirName[i] = 0;
        }
        for (i = 0; i < 512; i++)
            fileContent[i] = 0;

        do
        {
            ch = getchar();
        } while (ch == ' ' || ch == '\n');

        i = 0;
        while (ch != ' ' && ch != '\n')
        {
            command[i++] = ch;
            ch = getchar();
        }
        command[i] = '\0';

        while (ch == ' ' || ch == '\n')
            ch = getchar();
        i = 0;
        while (ch != ' ' && ch != '\n')
        {
            fileOrDirName[i++] = ch;
            ch = getchar();
        }
        fileOrDirName[i] = '\0';

        while (ch == ' ' && ch == '\n')
            ch = getchar();
        i = 0;
        ch = getchar();
        while (ch != '\n' && ch != EOF)
        {
            if (i != 0)
                ch = getchar();
            fileContent[i++] = ch;
        }
        fileContent[i] = '\0';

        if (str_cmp(command, "exit") == 0)
            break;
        else if (str_cmp(command, "mkdir") == 0)
            mkdirCommand(fileOrDirName);
        else if (str_cmp(command, "create") == 0)
            createCommand(fileOrDirName);
        else if (str_cmp(command, "write") == 0)
            writeCommand(fileOrDirName, fileContent);
        else if (str_cmp(command, "read") == 0)
            readCommand(fileOrDirName);
        else if (str_cmp(command, "delete") == 0)
            deleteCommand(fileOrDirName);
        else if (str_cmp(command, "rmdir") == 0)
            rmdirCommand(fileOrDirName);
        else if (str_cmp(command, "ls") == 0)
            lsCommand();
        else if (str_cmp(command, "cd") == 0)
            cdCommand(fileOrDirName);
        else if (str_cmp(command, "pwd") == 0)
        {
            pwdCommand(cwd);
            printf("\n");
        }
        else if (str_cmp(command, "df") == 0)
            dfCommand();
        else
            printf("Unknown command.\n");
    }

    printf("Exiting program...\n");
    return 0;
}
