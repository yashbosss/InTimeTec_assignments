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

    FILE *fp = fopen("data.txt", "w");
    fprintf(fp, "%d\n", numberOfElements);
    for (int i = 0; i < numberOfElements; i++)
        fprintf(fp, "%d ", arr[i]);
    fclose(fp);

    pid_t pid = fork();

    if (pid == 0) {
        fp = fopen("data.txt", "r");
        fscanf(fp, "%d", &numberOfElements);
        for (int i = 0; i < numberOfElements; i++)
            fscanf(fp, "%d", &arr[i]);
        fclose(fp);

        sort(arr, numberOfElements);

        fp = fopen("data.txt", "w");
        fprintf(fp, "%d\n", numberOfElements);
        for (int i = 0; i < numberOfElements; i++)
            fprintf(fp, "%d ", arr[i]);
        fclose(fp);

        exit(0);
    } else {
        wait(NULL);

        fp = fopen("data.txt", "r");
        fscanf(fp, "%d", &numberOfElements);
        for (int i = 0; i < numberOfElements; i++)
            fscanf(fp, "%d", &arr[i]);
        fclose(fp);

        printf("After sorting:\n");
        for (int i = 0; i < numberOfElements; i++)
            printf("%d ", arr[i]);
        printf("\n");
    }

    return 0;
}
