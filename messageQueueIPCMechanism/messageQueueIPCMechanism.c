#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <unistd.h>

struct message {
    long type;
    int numberOfElements;
    int arr[100];
};

void sort(int arr[], int numberOfElements) {
    for (int i = 0; i < numberOfElements - 1; i++) {
        for (int j = 0; j < numberOfElements - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

int main() {
    key_t key = ftok(".", 65);
    int msgid = msgget(key, 0666 | IPC_CREAT);

    struct message msg;
    msg.type = 1;

    printf("Enter number of elements: ");
    scanf("%d", &msg.numberOfElements);

    printf("Enter elements:\n");
    for (int i = 0; i < msg.numberOfElements; i++)
        scanf("%d", &msg.arr[i]);

    printf("Before sorting:\n");
    for (int i = 0; i < msg.numberOfElements; i++)
        printf("%d ", msg.arr[i]);
    printf("\n");

    pid_t pid = fork();

    if (pid == 0) {
        msgrcv(msgid, &msg, sizeof(msg) - sizeof(long), 1, 0);
        sort(msg.arr, msg.numberOfElements);
        msg.type = 2;
        msgsnd(msgid, &msg, sizeof(msg) - sizeof(long), 0);
        exit(0);
    } else {
        msgsnd(msgid, &msg, sizeof(msg) - sizeof(long), 0);
        msgrcv(msgid, &msg, sizeof(msg) - sizeof(long), 2, 0);

        printf("After sorting:\n");
        for (int i = 0; i < msg.numberOfElements; i++)
            printf("%d ", msg.arr[i]);
        printf("\n");

        wait(NULL);
        msgctl(msgid, IPC_RMID, NULL);
    }
    return 0;
}
