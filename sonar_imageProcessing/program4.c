#include <stdio.h>
#include <stdlib.h>

void transposeMatrix(int **sonarImageMatrix, int matrixSize)
{
    for (int i = 0; i < matrixSize; i++)
    {
        for (int j = i + 1; j < matrixSize; j++)
        {
            int temp = *(*(sonarImageMatrix + i) + j);
            *(*(sonarImageMatrix + i) + j) = *(*(sonarImageMatrix + j) + i);
            *(*(sonarImageMatrix + j) + i) = temp;
        }
    }
}

void reverseMatrixRows(int **sonarImageMatrix, int matrixSize)
{
    for (int i = 0; i < matrixSize; i++)
    {
        int *rowStart = *(sonarImageMatrix + i);
        int *rowEnd = *(sonarImageMatrix + i) + matrixSize - 1;

        while (rowStart < rowEnd)
        {
            int temp = *rowStart;
            *rowStart = *rowEnd;
            *rowEnd = temp;
            rowStart++;
            rowEnd--;
        }
    }
}

void rotate90Clockwise(int **sonarImageMatrix, int matrixSize)
{
    transposeMatrix(sonarImageMatrix, matrixSize);
    reverseMatrixRows(sonarImageMatrix, matrixSize);
}

void SmoothingFilter(int **sonarImageMatrix, int matrixSize)
{
    for (int i = 0; i < matrixSize; i++)
    {
        for (int j = 0; j < matrixSize; j++)
        {
            int sumOfNeighbour = 0, countNeighbour = 0;

            for (int x = i - 1; x <= i + 1; x++)
            {
                for (int y = j - 1; y <= j + 1; y++)
                {
                    if (x >= 0 && x < matrixSize && y >= 0 && y < matrixSize)
                    {
                        sumOfNeighbour += *(*(sonarImageMatrix + x) + y) % 1000;
                        countNeighbour++;
                    }
                }
            }

            *(*(sonarImageMatrix + i) + j) += (sumOfNeighbour / countNeighbour) * 1000;
        }
    }

    for (int i = 0; i < matrixSize; i++)
    {
        for (int j = 0; j < matrixSize; j++)
        {
            *(*(sonarImageMatrix + i) + j) /= 1000;
        }
    }
}

void printMatrix(int **sonarImageMatrix, int matrixSize)
{
    for (int i = 0; i < matrixSize; i++)
    {
        for (int j = 0; j < matrixSize; j++)
        {
            printf("%4d", *(*(sonarImageMatrix + i) + j));
        }
        printf("\n");
    }
}

void freeMatrix(int **sonarImageMatrix, int matrixSize)
{
    for (int i = 0; i < matrixSize; i++)
    {
        free(*(sonarImageMatrix + i));
    }
    free(sonarImageMatrix);
}

int main()
{
    int sonarImageSize;
    printf("Enter size of sonar Image between 2 to 10:\n");
    scanf("%d", &sonarImageSize);

    int **sonarImageMatrix = (int **)malloc(sonarImageSize * sizeof(int *));
    for (int i = 0; i < sonarImageSize; i++)
    {
        *(sonarImageMatrix + i) = (int *)malloc(sonarImageSize * sizeof(int));
    }

    srand(time(NULL));

    for (int i = 0; i < sonarImageSize; i++)
    {
        for (int j = 0; j < sonarImageSize; j++)
        {
            *(*(sonarImageMatrix + i) + j) = rand() % 256;
        }
    }

    printf("\nOriginal Matrix:\n");
    printMatrix(sonarImageMatrix, sonarImageSize);

    rotate90Clockwise(sonarImageMatrix, sonarImageSize);
    printf("\nRotated:\n");
    printMatrix(sonarImageMatrix, sonarImageSize);

    SmoothingFilter(sonarImageMatrix, sonarImageSize);
    printf("\nSmoothening Filter:\n");
    printMatrix(sonarImageMatrix, sonarImageSize);

    freeMatrix(sonarImageMatrix, sonarImageSize);
    return 0;
}
