#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

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
    int p1[2], p2[2];
    pipe(p1);
    pipe(p2);

    int numberOfElements;
    printf("Enter number of elements: ");
    scanf("%d", &numberOfElements);

    int arr[numberOfElements];
    printf("Enter elements:\n");
    for (int i = 0; i < numberOfElements; i++)
        scanf("%d", &arr[i]);

    printf("Before sorting:\n");
    for (int i = 0; i < numberOfElements; i++)
        printf("%d ", arr[i]);
    printf("\n");

    pid_t pid = fork();

    if (pid == 0) {
        read(p1[0], &numberOfElements, sizeof(int));
        read(p1[0], arr, sizeof(int) * numberOfElements);

        sort(arr, numberOfElements);

        write(p2[1], arr, sizeof(int) * numberOfElements);
        exit(0);
    } else {
        write(p1[1], &numberOfElements, sizeof(int));
        write(p1[1], arr, sizeof(int) * numberOfElements);

        wait(NULL);

        read(p2[0], arr, sizeof(int) * numberOfElements);

        printf("After sorting:\n");
        for (int i = 0; i < numberOfElements; i++)
            printf("%d ", arr[i]);
        printf("\n");
    }
    return 0;
}
