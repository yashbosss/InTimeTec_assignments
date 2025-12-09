#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define NAME_LEN 64

typedef enum
{
    NEW_PROCESS = 0,
    READY_PROCESS,
    RUNNING_PROCESS,
    WAITING_PROCESS,
    TERMINATED_PROCESS,
    KILLED_PROCESS
} processState;

typedef struct ProcessControlBlocks
{
    int processID;
    char name[NAME_LEN];
    int burst;
    int ioStarttart;
    int ioLen;

    int usedCPU;
    int usedIO;
    int execSince;
    int remainingIO;

    int arrival;
    int waiting;
    int completion;
    int totalIOTimeSpent;

    processState state;
    struct ProcessControlBlocks *next;
} ProcessControlBlocks;

typedef struct
{
    ProcessControlBlocks *head;
    ProcessControlBlocks *tail;
} ProcessQueue;

typedef struct
{
    int key;
    ProcessControlBlocks *process;
} Slot;

typedef struct
{
    int processID;
    int at;
} KillEvent;

ProcessQueue readyQ, waitQ, doneQ;
Slot *hashTable = NULL;
int hashSize = 0;

int totalProcess = 0;
int now = 0;
int doneCount = 0;

ProcessControlBlocks *running = NULL;

int readIntegers()
{
    char line[128];
    int x;

    while (1)
    {
        if (!fgets(line, sizeof(line), stdin))
            continue;
        if (sscanf(line, "%d", &x) == 1 && x > 0)
            return x;
        printf("Invalid input! Try again.\n");
    }
}

int readProcessLines(ProcessControlBlocks *p)
{
    char line[256];
    while (1)
    {
        if (!fgets(line, sizeof(line), stdin))
            continue;

        char name[NAME_LEN];
        int processID, cpu, ioStart, ioDuration;

        int f = sscanf(line, "%63s %d %d %d %d",
                       name, &processID, &cpu, &ioStart, &ioDuration);

        if (f == 5 && processID > 0 && cpu > 0 && ioStart >= -1 && ioDuration >= 0)
        {
            if (ioDuration == 0)
            {
                ioStart = 0;
                ioDuration = 0;
            }

            strncpy(p->name, name, NAME_LEN - 1);
            p->name[NAME_LEN - 1] = '\0';

            p->processID = processID;
            p->burst = cpu;
            p->ioStarttart = ioStart;
            p->ioLen = ioDuration;

            p->usedCPU = 0;
            p->usedIO = 0;
            p->execSince = 0;
            p->remainingIO = 0;
            p->arrival = 0;
            p->waiting = 0;
            p->completion = -1;
            p->totalIOTimeSpent = 0;
            p->state = READY_PROCESS;
            p->next = NULL;

            return 1;
        }
        printf("Invalid input! Try again.\n");
    }
}

int readKillEventLines(KillEvent *k)
{
    char line[128];
    while (1)
    {
        if (!fgets(line, sizeof(line), stdin))
            continue;

        char command[16];
        int processID, time;

        int f = sscanf(line, "%15s %d %d", command, &processID, &time);

        if (f == 3 && strcmp(command, "KILL") == 0 && processID > 0 && time >= 0)
        {
            k->processID = processID;
            k->at = time;
            return 1;
        }

        printf("Invalid input! Try again.\n");
    }
}

void initializeQueue(ProcessQueue *q) { q->head = q->tail = NULL; }

void pushIntoQueue(ProcessQueue *q, ProcessControlBlocks *p)
{
    p->next = NULL;
    if (!q->tail)
        q->head = q->tail = p;
    else
    {
        q->tail->next = p;
        q->tail = p;
    }
}

ProcessControlBlocks *popFromQueue(ProcessQueue *q)
{
    if (!q->head)
        return NULL;
    ProcessControlBlocks *p = q->head;
    q->head = p->next;
    if (!q->head)
        q->tail = NULL;
    p->next = NULL;
    return p;
}

ProcessControlBlocks *removeProcessIdfromQueue(ProcessQueue *q, int processID)
{
    ProcessControlBlocks *prev = NULL, *cur = q->head;

    while (cur)
    {
        if (cur->processID == processID)
        {
            if (!prev)
                q->head = cur->next;
            else
                prev->next = cur->next;

            if (q->tail == cur)
                q->tail = prev;

            cur->next = NULL;
            return cur;
        }
        prev = cur;
        cur = cur->next;
    }
    return NULL;
}

int hashIndex(int processID)
{
    unsigned int k = (unsigned int)processID;
    k *= 2654435761u;
    return (int)(k % hashSize);
}

void initializeHash(int size)
{
    hashSize = size;
    hashTable = malloc(sizeof(Slot) * hashSize);

    for (int i = 0; i < hashSize; i++)
    {
        hashTable[i].key = -1;
        hashTable[i].process = NULL;
    }
}

void hashPut(ProcessControlBlocks *p)
{
    int idx = hashIndex(p->processID);

    while (hashTable[idx].key != -1)
        idx = (idx + 1) % hashSize;

    hashTable[idx].key = p->processID;
    hashTable[idx].process = p;
}

ProcessControlBlocks *hashGet(int processID)
{
    int idx = hashIndex(processID);
    int start = idx;

    while (hashTable[idx].key != -1)
    {
        if (hashTable[idx].key == processID)
            return hashTable[idx].process;
        idx = (idx + 1) % hashSize;
        if (idx == start)
            break;
    }
    return NULL;
}

void progressIO()
{
    ProcessControlBlocks *cur = waitQ.head, *prev = NULL;

    while (cur)
    {
        ProcessControlBlocks *temp = cur->next;
        cur->remainingIO--;
        cur->usedIO++;
        cur->totalIOTimeSpent++;

        if (cur->remainingIO <= 0)
        {
            cur->state = READY_PROCESS;
            cur->execSince = 0;

            if (!prev)
                waitQ.head = temp;
            else
                prev->next = temp;

            if (waitQ.tail == cur)
                waitQ.tail = prev;

            cur->next = NULL;
            pushIntoQueue(&readyQ, cur);

            printf("time %d: PID %d IO complete -> READY\n", now + 1, cur->processID);
        }
        else
            prev = cur;

        cur = temp;
    }
}

void processKills(KillEvent *event, int total, int *ptr)
{
    while (*ptr < total && event[*ptr].at == now)
    {
        int id = event[*ptr].processID;
        ProcessControlBlocks *temp = hashGet(id);

        if (temp && temp->state != TERMINATED_PROCESS && temp->state != KILLED_PROCESS)
        {
            if (running == temp)
            {
                running = NULL;
                temp->state = KILLED_PROCESS;
                temp->completion = now;

                pushIntoQueue(&doneQ, temp);
                doneCount++;

                printf("time %d: PID %d KILLED\n", now, id);
            }
            else
            {
                ProcessControlBlocks *r = removeProcessIdfromQueue(&readyQ, id);
                if (r)
                {
                    r->state = KILLED_PROCESS;
                    r->completion = now;
                    pushIntoQueue(&doneQ, r);
                    doneCount++;

                    printf("time %d: PID %d KILLED (READY)\n", now, id);
                }
                else
                {
                    ProcessControlBlocks *w = removeProcessIdfromQueue(&waitQ, id);
                    if (w)
                    {
                        w->state = KILLED_PROCESS;
                        w->completion = now;

                        pushIntoQueue(&doneQ, w);
                        doneCount++;

                        printf("time %d: PID %d KILLED (WAITING)\n", now, id);
                    }
                }
            }
        }
        (*ptr)++;
    }
}

void simulateTick()
{
    if (running)
    {
        running->usedCPU++;
        running->execSince++;

        printf("time %d: Running PID %d (cpuUsed=%d)\n",
               now, running->processID, running->usedCPU);

        if (running->ioLen > 0 &&
            running->execSince == running->ioStarttart &&
            running->usedIO == 0)
        {
            running->remainingIO = running->ioLen;
            running->state = WAITING_PROCESS;
            pushIntoQueue(&waitQ, running);

            printf("time %d: PID %d moved to WAITING (IO start)\n",
                   now + 1, running->processID);

            running = NULL;
        }

        if (running && running->usedCPU >= running->burst)
        {
            running->state = TERMINATED_PROCESS;
            running->completion = now + 1;
            pushIntoQueue(&doneQ, running);
            doneCount++;

            printf("time %d: PID %d TERMINATED\n",
                   now + 1, running->processID);

            running = NULL;
        }
    }

    progressIO();
}

void incrementReadyWaits()
{
    ProcessControlBlocks *cur = readyQ.head;
    while (cur)
    {
        cur->waiting++;
        cur = cur->next;
    }
}

void simulate(KillEvent *event, int killCount)
{
    int ptr = 0;
    now = 0;
    doneCount = 0;

    running = NULL;

    while (doneCount < totalProcess)
    {
        processKills(event, killCount, &ptr);

        if (!running)
        {
            running = popFromQueue(&readyQ);
            if (running)
            {
                running->state = RUNNING_PROCESS;
                printf("time %d: PID %d scheduled\n", now, running->processID);
            }
        }

        incrementReadyWaits();

        simulateTick();

        now++;
    }
}

void displayResult()
{
    printf("\n========= FINAL RESULTS =========\n");
    printf("PID\tName\tCPU\tIO\tStatus\t\tTurnaround\tWaiting\n");

    ProcessControlBlocks *arr[256];
    int n = 0;

    ProcessControlBlocks *cur = doneQ.head;
    while (cur)
    {
        arr[n++] = cur;
        cur = cur->next;
    }

    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - i - 1; j++)
            if (arr[j]->processID > arr[j + 1]->processID)
            {
                ProcessControlBlocks *tmp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = tmp;
            }

    for (int i = 0; i < n; i++)
    {
        ProcessControlBlocks *p = arr[i];

        if (p->state == KILLED_PROCESS)
        {
            printf("%d\t%s\t%d\t%d\tKILLED at %d\t-\t\t-\n",
                   p->processID, p->name, p->burst, p->totalIOTimeSpent, p->completion);
            continue;
        }

        int tat = p->completion - p->arrival;

        int wait = tat - p->burst - p->totalIOTimeSpent;

        if (wait < 0)
        {
            wait = 0;
        }

        int io_output = p->totalIOTimeSpent;

        printf("%d\t%s\t%d\t%d\tOK\t\t%d\t\t%d\n",
               p->processID, p->name,
               p->burst,
               io_output,
               tat,
               wait);
    }
}

void freeMemory(KillEvent *event)
{
    ProcessControlBlocks *temp;

    while ((temp = doneQ.head))
    {
        doneQ.head = temp->next;
        free(temp);
    }
    while ((temp = readyQ.head))
    {
        doneQ.head = temp->next;
        free(temp);
    }
    while ((temp = waitQ.head))
    {
        waitQ.head = temp->next;
        free(temp);
    }

    if (hashTable)
        free(hashTable);
    if (event)
        free(event);
}

int main()
{
    initializeQueue(&readyQ);
    initializeQueue(&waitQ);
    initializeQueue(&doneQ);

    printf("Enter number of processes: ");
    totalProcess = readIntegers();

    initializeHash(totalProcess * 2);

    printf("\nEnter each process as: <name> <processID> <cpu> <ioStarttart> <ioDurationuration>\n");
    for (int i = 0; i < totalProcess; i++)
    {
        ProcessControlBlocks *p = malloc(sizeof(ProcessControlBlocks));
        readProcessLines(p);
        hashPut(p);
        pushIntoQueue(&readyQ, p);
    }

    printf("\nEnter number of kill events: ");
    int killCount;
    scanf("%d", &killCount);
    char c;
    while ((c = getchar()) != '\n' && c != EOF);
    printf("\n");
    KillEvent *event = NULL;
    if (killCount > 0)
    {
        printf("Enter each kill event as: KILL <processID> <time>\n");
        event = malloc(sizeof(KillEvent) * killCount);
        for (int i = 0; i < killCount; i++)
            readKillEventLines(&event[i]);
    }

    simulate(event, killCount);
    displayResult();
    freeMemory(event);
    return 0;
}