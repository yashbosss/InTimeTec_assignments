#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_LENGTH 50
#define HASH_SIZE 2009

typedef struct Node {
    int key;
    char data[MAX_LENGTH];
    struct Node *prev;
    struct Node *next;
} Node;

typedef struct Queue {
    Node *front;  
    Node *rear;   
} Queue;

typedef struct Hash {
    int key;      
    Node *node;  
} Hash;

Hash *hashTable;

int strCmp(const char *a, const char *b)
{
    int i = 0;
    while (a[i] != '\0' && b[i] != '\0')
    {
        if (a[i] != b[i]) {
            return (a[i] > b[i]) ? 1 : -1;
        }
        i++;
    }
    if (a[i] == '\0' && b[i] == '\0')
        return 0;
    return (a[i] == '\0') ? -1 : 1;
}

void strCopy(char *dest, char *src, int maxLen) {
    int i = 0;
    while (src[i] != '\0' && i < maxLen - 1) {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

Node* createNode(int key, char *data) {
    Node *nodeToInsert = (Node*)malloc(sizeof(Node));
    if (!nodeToInsert) return NULL;

    nodeToInsert->key = key;
    strCopy(nodeToInsert->data, data, MAX_LENGTH);
    nodeToInsert->prev = NULL;
    nodeToInsert->next = NULL;
    return nodeToInsert;
}

void initializeHash(int size) {
    hashTable = (Hash*)malloc(HASH_SIZE * sizeof(Hash));

    for (int i = 0; i < size; i++) {
        hashTable[i].key = -1;  
        hashTable[i].node = NULL;
    }
}

int hashFunction(int key) {
    return key % HASH_SIZE;
}

int searchKeyIndex(int key) {
    int idx = hashFunction(key);

    while (hashTable[idx].key != -1 && hashTable[idx].key != key)
        idx = (idx + 1) % HASH_SIZE;

    return idx;
}

void makeMRU(Queue *queue, Node *toBeMRU) {
    if (queue->front == toBeMRU)
        return;  

    if (toBeMRU->prev)
        toBeMRU->prev->next = toBeMRU->next;

    if (toBeMRU->next)
        toBeMRU->next->prev = toBeMRU->prev;

    if (queue->rear == toBeMRU)
        queue->rear = toBeMRU->prev;

    toBeMRU->prev = NULL;
    toBeMRU->next = queue->front;
    queue->front->prev = toBeMRU;
    queue->front = toBeMRU;
}

void insertAtFront(Queue *queue, Node *toInsert) {
    toInsert->prev = NULL;
    toInsert->next = queue->front;

    if (queue->front)
        queue->front->prev = toInsert;

    queue->front = toInsert;

    if (queue->rear == NULL)
        queue->rear = toInsert;
}

void removeRear(Queue *queue) {
    Node *LRU = queue->rear;
    if (!LRU) return;

    int key = LRU->key;

    if (LRU->prev) {
        queue->rear = LRU->prev;
        queue->rear->next = NULL;
    } else {
        queue->front = queue->rear = NULL;
    }

    int idx = searchKeyIndex(key);
    hashTable[idx].key = -1;
    hashTable[idx].node = NULL;

    free(LRU);
}

Node* hashGetNode(int key) {
    int idx = hashFunction(key);

    while (hashTable[idx].key != -1) {
        if (hashTable[idx].key == key)
            return hashTable[idx].node;
        idx = (idx + 1) % HASH_SIZE;
    }
    return NULL;
}

char* getValue(Queue *queue, int key) {
    Node *temp = hashGetNode(key);
    if (!temp) return NULL;

    makeMRU(queue, temp);
    return temp->data;
}

void putValue(Queue *queue, int capacity, int *currentSize, int key, char *data) {

    Node *checkNode = hashGetNode(key);

    if (checkNode) {
        strCopy(checkNode->data, data, MAX_LENGTH);
        makeMRU(queue, checkNode);
        return;
    }

    Node *newNode = createNode(key, data);
    insertAtFront(queue, newNode);

    int slot = searchKeyIndex(key);
    hashTable[slot].key = key;
    hashTable[slot].node = newNode;

    (*currentSize)++;

    if (*currentSize > capacity) {
        removeRear(queue);
        (*currentSize)--;
    }
}

void freeQueue(Queue *queue) {
    Node *temp = queue->front;

    while (temp != NULL) {
        Node *next = temp->next;
        free(temp);
        temp = next;
    }

    queue->front = NULL;
    queue->rear = NULL;
}

void freeHashTable() {
    free(hashTable);
}


int main() {
    Queue q = {NULL, NULL};
    int capacity = 0, currentSize = 0;
    while (1) {
        printf("Enter cache size: ");

        if (scanf("%d", &capacity) != 1 || capacity <= 0) {
            printf("Invalid cache size. Please enter a positive integer.\n");
            scanf("%*s");
            continue;
        }
        break;
    }

    initializeHash(capacity * 5);

    printf("\nLRU Cache initialized with capacity %d\n", capacity);
    printf("----------------------------------------------------\n");
    printf("Available Commands:\n");
    printf("  put <key> <value>  - Insert/update a key-value pair\n");
    printf("  get <key>          - Retrieve value for a key\n");
    printf("  exit               - Quit the program\n");
    printf("----------------------------------------------------\n\n");
    char cmd[20];

    while (1) {

        printf("Enter command (put/get/exit): ");

        if (scanf("%s", cmd) != 1)
            continue;

        if (strCmp(cmd, "put") == 0) {

            int key;
            char val[MAX_LENGTH];

            printf("Enter key (integer): ");
            if (scanf("%d", &key) != 1) {
                printf("Invalid key. Key must be an integer.\n");
                scanf("%*s"); 
                continue;
            }

            printf("Enter value (string): ");
            scanf("%s", val);

            putValue(&q, capacity, &currentSize, key, val);

            printf("Inserted/Updated key %d with value \"%s\".\n", key, val);
        }

        else if (strCmp(cmd, "get") == 0) {

            int key;

            printf("Enter key (integer): ");
            if (scanf("%d", &key) != 1) {
                printf("Invalid key. Key must be an integer.\n");
                scanf("%*s");
                continue;
            }

            char *ans = getValue(&q, key);

            if (ans)
                printf("Value: %s\n", ans);
            else
                printf("NULL\n");
        }

        else if (strCmp(cmd, "exit") == 0) {
            printf("Exiting LRU Cache program.\n");
            freeQueue(&q);
            freeHashTable();
            break;
        }

        else {
            printf("Unknown command.\n");
            printf("Valid commands: put, get, exit\n");
        }

        printf("\n"); 
    }

    return 0;
}
