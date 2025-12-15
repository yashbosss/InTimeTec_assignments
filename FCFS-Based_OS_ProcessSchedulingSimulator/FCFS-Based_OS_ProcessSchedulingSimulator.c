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
    char processName[NAME_LEN];
    int cpuBurstTime;
    int ioStartTime;
    int ioDurationTime;

    int cpuUsed;
    int ioUsed;
    int cpuSinceStart;
    int ioRemaining;

    int arrivalTime;
    int waitingTime;
    int completionTime;
    int ioTotalTime;

    int ioJustStartedFlag; 

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
    int killedAtTime;
} KillEvent;

ProcessQueue readyQueue, waitQueue, terminationQueue;
Slot *hashTable = NULL;
int hashSize = 0;

int totalProcesses = 0;
int currentTime = 0;
int completedProcessCount = 0;

ProcessControlBlocks *runningProcessTracker = NULL;

int readIntegers()
{
    char bufferLine[128];
    int inputInteger;

    while (1)
    {
        if (!fgets(bufferLine, sizeof(bufferLine), stdin))
            continue;
        if (sscanf(bufferLine, "%d", &inputInteger) == 1 && inputInteger > 0)
            return inputInteger;
        printf("Invalid input! Try again.\n");
    }
}

int readProcessLines(ProcessControlBlocks *process)
{
    char bufferLine[256];
    while (1)
    {
        if (!fgets(bufferLine, sizeof(bufferLine), stdin))
            continue;

        char processName[NAME_LEN];
        int processID, cpuBurstTime, ioStartTime, ioDurationTime;

        int f = sscanf(bufferLine, "%63s %d %d %d %d",
                       processName, &processID, &cpuBurstTime, &ioStartTime, &ioDurationTime);

        if (f == 5 && processID > 0 && cpuBurstTime > 0 && ioStartTime >= -1 && ioDurationTime >= 0)
        {
            if (ioDurationTime == 0)
            {
                ioStartTime = 0;
                ioDurationTime = 0;
            }

            strncpy(process->processName, processName, NAME_LEN - 1);
            process->processName[NAME_LEN - 1] = '\0';

            process->processID = processID;
            process->cpuBurstTime = cpuBurstTime;
            process->ioStartTime = ioStartTime;
            process->ioDurationTime = ioDurationTime;

            process->cpuUsed = 0;
            process->ioUsed = 0;
            process->cpuSinceStart = 0;
            process->ioRemaining = 0;
            process->arrivalTime = 0;
            process->waitingTime = 0;
            process->completionTime = -1;
            process->ioTotalTime = 0;
            process->state = READY_PROCESS;
            process->next = NULL;
            process->ioJustStartedFlag = 0; 

            return 1;
        }
        printf("Invalid input! Try again.\n");
    }
}

int readKillEventLines(KillEvent *event)
{
    char line[128];
    while (1)
    {
        if (!fgets(line, sizeof(line), stdin))
            continue;

        char command[16];
        int processID, time;

        int flag = sscanf(line, "%15s %d %d", command, &processID, &time);

        if (flag == 3 && strcmp(command, "KILL") == 0 && processID > 0 && time >= 0)
        {
            event->processID = processID;
            event->killedAtTime = time;
            return 1;
        }

        printf("Invalid input! Try again.\n");
    }
}

void initializeQueue(ProcessQueue *queue) { queue->head = queue->tail = NULL; }

void pushIntoQueue(ProcessQueue *queue, ProcessControlBlocks *process)
{
    process->next = NULL;
    if (!queue->tail)
        queue->head = queue->tail = process;
    else
    {
        queue->tail->next = process;
        queue->tail = process;
    }
}

ProcessControlBlocks *popFromQueue(ProcessQueue *queue)
{
    if (!queue->head)
        return NULL;
    ProcessControlBlocks *process = queue->head;
    queue->head = process->next;
    if (!queue->head)
        queue->tail = NULL;
    process->next = NULL;
    return process;
}

ProcessControlBlocks *removeProcessIdfromQueue(ProcessQueue *queue, int processID)
{
    ProcessControlBlocks *prev = NULL, *cur = queue->head;

    while (cur)
    {
        if (cur->processID == processID)
        {
            if (!prev)
                queue->head = cur->next;
            else
                prev->next = cur->next;

            if (queue->tail == cur)
                queue->tail = prev;

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
    unsigned int tempVariable = (unsigned int)processID;
    tempVariable *= 2654435761u;
    return (int)(tempVariable % hashSize);
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

void hashPut(ProcessControlBlocks *process)
{
    int index = hashIndex(process->processID);
    int start = index;
    while (hashTable[index].key != -1) {
        if (hashTable[index].key == process->processID)
        return;
        
        index = (index + 1) % hashSize;
        if (index == start) {
            return;
        }
    }
    hashTable[index].key = process->processID;
    hashTable[index].process = process;
}

ProcessControlBlocks *hashGet(int processID)
{
    int index = hashIndex(processID);
    int start = index;

    while (hashTable[index].key != -1)
    {
        if (hashTable[index].key == processID)
            return hashTable[index].process;
        index = (index + 1) % hashSize;
        if (index == start)
            break;
    }
    return NULL;
}

void progressIO()
{
    ProcessControlBlocks *cur = waitQueue.head, *prev = NULL;

    while (cur)
    {
        ProcessControlBlocks *temp = cur->next;
        if (cur->ioJustStartedFlag)
        {
            cur->ioJustStartedFlag = 0;
        }
        else
        {
            cur->ioRemaining--;
            cur->ioUsed++;
            cur->ioTotalTime++;
        }

        if (cur->ioRemaining <= 0)
        {
            cur->state = READY_PROCESS;

            if (!prev)
                waitQueue.head = temp;
            else
                prev->next = temp;

            if (waitQueue.tail == cur)
                waitQueue.tail = prev;

            cur->next = NULL;
            pushIntoQueue(&readyQueue, cur);

            printf("time %d: PID %d IO complete -> READY\n", currentTime + 1, cur->processID);
        }
        else
            prev = cur;

        cur = temp;
    }
}

void processKills(KillEvent *event, int total, int *ptr)
{
    while (*ptr < total && event[*ptr].killedAtTime == currentTime)
    {
        int id = event[*ptr].processID;
        ProcessControlBlocks *temp = hashGet(id);

        if (temp && temp->state != TERMINATED_PROCESS && temp->state != KILLED_PROCESS)
        {
            if (runningProcessTracker == temp)
            {
                runningProcessTracker = NULL;
                temp->state = KILLED_PROCESS;
                temp->completionTime = currentTime;

                pushIntoQueue(&terminationQueue, temp);
                completedProcessCount++;

                printf("time %d: PID %d KILLED\n", currentTime, id);
            }
            else
            {
                ProcessControlBlocks *process = removeProcessIdfromQueue(&readyQueue, id);
                if (process)
                {
                    process->state = KILLED_PROCESS;
                    process->completionTime = currentTime;
                    pushIntoQueue(&terminationQueue, process);
                    completedProcessCount++;

                    printf("time %d: PID %d KILLED (READY)\n", currentTime, id);
                }
                else
                {
                    ProcessControlBlocks *process = removeProcessIdfromQueue(&waitQueue, id);
                    if (process)
                    {
                        process->state = KILLED_PROCESS;
                        process->completionTime = currentTime;

                        pushIntoQueue(&terminationQueue, process);
                        completedProcessCount++;

                        printf("time %d: PID %d KILLED (WAITING)\n", currentTime, id);
                    }
                }
            }
        }
        (*ptr)++;
    }
}

void simulateTick()
{
    if (runningProcessTracker)
    {
        runningProcessTracker->cpuUsed++;
        runningProcessTracker->cpuSinceStart++;

        printf("time %d: Running PID %d (cpuUsed=%d)\n",
               currentTime, runningProcessTracker->processID, runningProcessTracker->cpuUsed);

        if (runningProcessTracker->ioDurationTime > 0 &&
            runningProcessTracker->cpuSinceStart == runningProcessTracker->ioStartTime)
        {
            runningProcessTracker->ioRemaining = runningProcessTracker->ioDurationTime;
            runningProcessTracker->state = WAITING_PROCESS;

            runningProcessTracker->ioJustStartedFlag = 1;

            pushIntoQueue(&waitQueue, runningProcessTracker);

            printf("time %d: PID %d moved to WAITING (IO start)\n",
                   currentTime + 1, runningProcessTracker->processID);

            runningProcessTracker = NULL;
        }

        if (runningProcessTracker && runningProcessTracker->cpuUsed >= runningProcessTracker->cpuBurstTime)
        {
            runningProcessTracker->state = TERMINATED_PROCESS;
            runningProcessTracker->completionTime = currentTime + 1;
            pushIntoQueue(&terminationQueue, runningProcessTracker);
            completedProcessCount++;

            printf("time %d: PID %d TERMINATED\n",
                   currentTime + 1, runningProcessTracker->processID);

            runningProcessTracker = NULL;
        }
    }

    progressIO();
}

void simulate(KillEvent *event, int killCount)
{
    int ptr = 0;
    currentTime = 0;
    completedProcessCount = 0;

    runningProcessTracker = NULL;

    while (completedProcessCount < totalProcesses)
    {
        processKills(event, killCount, &ptr);

        if (!runningProcessTracker)
        {
            runningProcessTracker = popFromQueue(&readyQueue);
            if (runningProcessTracker)
            {
                runningProcessTracker->state = RUNNING_PROCESS;
                printf("time %d: PID %d scheduled\n", currentTime, runningProcessTracker->processID);
            }
        }

        simulateTick();

        currentTime++;
    }
}

void displayResult(int totalProcess)
{
    printf("\n========= FINAL RESULTS =========\n");
    printf("PID\tName\tCPU\tIO\tStatus\t\tTurnaround\tWaiting\n");

    ProcessControlBlocks **arr = malloc(sizeof(ProcessControlBlocks*) * totalProcess);
    int terminatedCount = 0;

    ProcessControlBlocks *cur = terminationQueue.head;
    while (cur)
    {
        arr[terminatedCount++] = cur;
        cur = cur->next;
    }

    for (int i = 0; i < terminatedCount - 1; i++)
        for (int j = 0; j < terminatedCount - i - 1; j++)
            if (arr[j]->processID > arr[j + 1]->processID)
            {
                ProcessControlBlocks *tmp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = tmp;
            }

    for (int i = 0; i < terminatedCount; i++)
    {
        ProcessControlBlocks *process = arr[i];

        if (process->state == KILLED_PROCESS)
        {
            printf("%d\t%s\t%d\t%d\tKILLED AT %d\t-\t\t-\n",
                   process->processID, process->processName, process->cpuBurstTime, process->ioTotalTime, process->completionTime);
            continue;
        }

        int turnAroundTime = process->completionTime - process->arrivalTime;
        int waitTime = turnAroundTime - process->cpuBurstTime;

        if (waitTime < 0)
        {
            waitTime = 0;
        }

        int io_output = process->ioTotalTime;

        printf("%d\t%s\t%d\t%d\tOK\t\t%d\t\t%d\n",
               process->processID, process->processName,
               process->cpuBurstTime,
               io_output,
               turnAroundTime,
               waitTime);
    }
    free(arr);
}

void freeMemory(KillEvent *event)
{
    ProcessControlBlocks *temp;

    while ((temp = terminationQueue.head))
    {
        terminationQueue.head = temp->next;
        free(temp);
    }
    while ((temp = readyQueue.head))
    {
        readyQueue.head = temp->next;
        free(temp);
    }
    while ((temp = waitQueue.head))
    {
        waitQueue.head = temp->next;
        free(temp);
    }

    if (hashTable)
        free(hashTable);
    if (event)
        free(event);
}

int main()
{
    initializeQueue(&readyQueue);
    initializeQueue(&waitQueue);
    initializeQueue(&terminationQueue);

    printf("Enter number of processes: ");
    totalProcesses = readIntegers();

    initializeHash(totalProcesses * 2);

    printf("\nEnter each process as: <processName> <processID> <cpu> <ioStartTime> <ioDurationTime>\n");
    for (int i = 0; i < totalProcesses; i++)
    {
        ProcessControlBlocks *p = malloc(sizeof(ProcessControlBlocks));
        readProcessLines(p);
        hashPut(p);
        pushIntoQueue(&readyQueue, p);
    }

    printf("\nEnter number of kill events: ");
    int totalKillEvents;
    scanf("%d", &totalKillEvents);
    char c;
    while ((c = getchar()) != '\n' && c != EOF);
    printf("\n");
    KillEvent *event = NULL;
    if (totalKillEvents > 0)
    {
        printf("Enter each kill event as: KILL <processID> <time>\n");
        event = malloc(sizeof(KillEvent) * totalKillEvents);
        for (int i = 0; i < totalKillEvents; i++)
            readKillEventLines(&event[i]);
    }

    simulate(event, totalKillEvents);
    displayResult(totalProcesses);
    freeMemory(event);
    return 0;
}
