#include <stdio.h>

struct studentData
{
    int rollNumber;
    char name[100];
    int marksSub1;
    int marksSub2;
    int marksSub3;
};

int isValidName(char name[])
{
    int i = 0;
    while (name[i] != '\0')
    {
        if (name[i] >= '0' && name[i] <= '9')
        {
            return 0;
        }
        i++;
    }
    return 1;
}

float calculatingAverageMarks(int sub1Marks, int sub2Marks, int sub3Marks)
{
    int Total = sub1Marks + sub2Marks + sub3Marks;
    float average = Total / 3.0;
    return average;
}

void printGrade(int average)
{
    if (average >= 85)
    {
        printf("Grade: 'A'");
    }
    else if (average >= 70 && average < 85)
    {
        printf("Grade: 'B'");
    }
    else if (average >= 50 && average < 70)
    {
        printf("Grade: 'C'");
    }
    else if (average >= 35 && average < 50)
    {
        printf("Grade: 'D'");
    }
    else
    {
        printf("Grade: 'F'");
    }
}

void printStudentPerformance(int average)
{
    if (average >= 85)
    {
        printf("*****");
    }
    else if (average >= 70 && average < 85)
    {
        printf("****");
    }
    else if (average >= 50 && average < 70)
    {
        printf("***");
    }
    else if (average >= 35 && average < 50)
    {
        printf("**");
    }
}

void printStudentRoll(struct studentData s[], int serialNumber, int totalStudentCount)
{
    if (serialNumber == totalStudentCount)
    {
        return;
    }
    printf("%d ", s[serialNumber].rollNumber);
    printStudentRoll(s, serialNumber + 1, totalStudentCount);
}

void printStudentData(struct studentData s[], int totalStudentCount)
{
    printf("\n");
    for (int i = 0; i < totalStudentCount; i++)
    {
        printf("Roll: %d\n", s[i].rollNumber);
        printf("Name: %s\n", s[i].name);
        printf("Total: %d\n", s[i].marksSub1 + s[i].marksSub2 + s[i].marksSub3);
        printf("Average: %f\n", calculatingAverageMarks(s[i].marksSub1, s[i].marksSub2, s[i].marksSub3));

        // for integer required in printing grade and performance stars
        int average = calculatingAverageMarks(s[i].marksSub1, s[i].marksSub2, s[i].marksSub3);

        printf("Grade: ");
        printGrade(average);
        printf("\n");
        if (average >= 35)
        {
            printStudentPerformance(average);
        }
        printf("\n\n");
    }
}

int main()
{
    int totalNumberOfStudents;
    printf("Enter Number of students: ");
    scanf("%d", &totalNumberOfStudents);
    printf("\n\n");
    struct studentData s[totalNumberOfStudents];
    int i = 0;
    int n = totalNumberOfStudents;
    while (i < n)
    {
        printf("Enter Roll, name and marks for three subjects for student %d:\n", i + 1);

        if (scanf("%d", &s[i].rollNumber) + scanf("%s", s[i].name) + scanf("%d", &s[i].marksSub1) + scanf("%d", &s[i].marksSub2) + scanf("%d", &s[i].marksSub3) != 5)
        {
            printf("Invalid input! Retry\n");
            while (getchar() != '\n')
                ;
            continue;
        }

        if (s[i].rollNumber <= 0 ||
            s[i].marksSub1 < 0 || s[i].marksSub1 > 100 ||
            s[i].marksSub2 < 0 || s[i].marksSub2 > 100 ||
            s[i].marksSub3 < 0 || s[i].marksSub3 > 100 ||
            !isValidName(s[i].name))
        {
            printf("Invalid input! Retry\n\n");
            while (getchar() != '\n')
                ;
        }
        else
        {
            i++;
        }
    }

    printStudentData(s, n);
    i = 0;
    printf("List of Roll Numbers: ");
    printStudentRoll(s, i, n);
}