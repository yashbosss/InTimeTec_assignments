#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>

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
    int shmid = shmget(IPC_PRIVATE, 1024, 0666 | IPC_CREAT);
    int *data = (int *)shmat(shmid, NULL, 0);

    int numberOfElements;
    printf("Enter number of elements: ");
    scanf("%d", &numberOfElements);

    data[0] = numberOfElements;
    printf("Enter elements:\n");
    for (int i = 0; i < numberOfElements; i++)
        scanf("%d", &data[i + 1]);

    printf("Before sorting:\n");
    for (int i = 0; i < numberOfElements; i++)
        printf("%d ", data[i + 1]);
    printf("\n");

    pid_t pid = fork();

    if (pid == 0) {
        sort(&data[1], data[0]);
        exit(0);
    } else {
        wait(NULL);

        printf("After sorting:\n");
        for (int i = 0; i < numberOfElements; i++)
            printf("%d ", data[i + 1]);
        printf("\n");

        shmdt(data);
        shmctl(shmid, IPC_RMID, NULL);
    }
    return 0;
}
