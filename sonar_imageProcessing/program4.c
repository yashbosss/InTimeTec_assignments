#include <stdio.h>
#include <stdlib.h>

void transposeMatrix(int* sonarImageMatrix[][10], int matrixSize) {
    for (int i = 0; i < matrixSize; i++) {
        for (int j = i + 1; j < matrixSize; j++) {
            int* temp = sonarImageMatrix[i][j];
            sonarImageMatrix[i][j] = sonarImageMatrix[j][i];
            sonarImageMatrix[j][i] = temp;
        }
    }
}

void reverseMatrixRows(int* sonarImageMatrix[][10], int matrixSize) {
    for (int i = 0; i < matrixSize; i++) {
        for (int j = 0; j < matrixSize / 2; j++) {
            int* temp = sonarImageMatrix[i][j];
            sonarImageMatrix[i][j] = sonarImageMatrix[i][matrixSize - 1 - j];
            sonarImageMatrix[i][matrixSize - 1 - j] = temp;
        }
    }
}

void rotate90Clockwise(int* sonarImageMatrix[][10], int matrixSize) {
    transposeMatrix(sonarImageMatrix, matrixSize);
    reverseMatrixRows(sonarImageMatrix, matrixSize);
}

void smoothMatrix(int* sonarImageMatrix[][10], int matrixSize) {
    for (int i = 0; i < matrixSize; i++) {
        for (int j = 0; j < matrixSize; j++) {
            int sum = 0, count = 0;

            for (int di = -1; di <= 1; di++) {
                for (int dj = -1; dj <= 1; dj++) {
                    int ni = i + di;
                    int nj = j + dj;
                    if (ni >= 0 && ni < matrixSize && nj >= 0 && nj < matrixSize) {
                        sum += *sonarImageMatrix[ni][nj] % 1000;
                        count++;
                    }
                }
            }

            int avg = sum / count;
            *sonarImageMatrix[i][j] += avg * 1000;
        }
    }

    for (int i = 0; i < matrixSize; i++) {
        for (int j = 0; j < matrixSize; j++) {
            *sonarImageMatrix[i][j] = *sonarImageMatrix[i][j] / 1000;
        }
    }
}

void printMatrix(int* sonarImageMatrix[][10], int matrixSize) {
    for (int i = 0; i < matrixSize; i++) {
        for (int j = 0; j < matrixSize; j++) {
            printf("%3d ", *sonarImageMatrix[i][j]);
        }
        printf("\n");
    }
}

void freeMatrix(int* sonarImageMatrix[][10], int matrixSize) {
    for (int i = 0; i < matrixSize; i++) {
        for (int j = 0; j < matrixSize; j++) {
            free(sonarImageMatrix[i][j]);
        }
    }
}

int main() {
    int sonarImageSize;
    printf("Enter size of sonar Image between 2 to 10:\n");
    scanf("%d", &sonarImageSize);

    int* sonarImageMatrix[10][10];
    int intensityValue, checkInputBuffer;

    for (int i = 0; i < sonarImageSize; i++) {
        for (int j = 0; j < sonarImageSize; j++) {
            while (1) {
                printf("Enter intensityValue for index [%d][%d] : ", i, j);
                if (scanf("%d", &intensityValue) == 1 && intensityValue >= 0 && intensityValue <= 255) {
                    while ((checkInputBuffer = getchar()) != '\n' && checkInputBuffer != EOF);
                    sonarImageMatrix[i][j] = (int*)malloc(sizeof(int));
                    *sonarImageMatrix[i][j] = intensityValue;
                    break;
                } else {
                    printf("Invalid input. Try again.\n");
                    while ((checkInputBuffer = getchar()) != '\n' && checkInputBuffer != EOF);
                }
            }
        }
    }

    printf("\nOriginal:\n");
    printMatrix(sonarImageMatrix, sonarImageSize);

    rotate90Clockwise(sonarImageMatrix, sonarImageSize);
    printf("\nRotated:\n");
    printMatrix(sonarImageMatrix, sonarImageSize);

    smoothMatrix(sonarImageMatrix, sonarImageSize);
    printf("\nSmoothing Filter:\n");
    printMatrix(sonarImageMatrix, sonarImageSize);

    freeMatrix(sonarImageMatrix, sonarImageSize);
    return 0;
}
