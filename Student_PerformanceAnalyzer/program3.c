#include <stdio.h>

int n;

struct studentData
{
    int studentRollNumber;
    char studentName[100];
    int studentMarksSub1;
    int studentMakrsSub2;
    int studentMarksSub3;
};

int isValidName(char name[]) {
    int i = 0;
    while (name[i] != '\0') {
        if (name[i] >= '0' && name[i] <= '9') {
            return 0;
        }
        i++;
    }
    return 1;
}


// for calculating average marks
int averageMarks(int sub1Marks, int sub2Marks, int sub3Marks)
{
    int Total = sub1Marks + sub2Marks + sub3Marks;
    float average = Total / 3;
    return average;
}

// for calculating grade of student
void studentGrade(int average)
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

// for giving stars according to student performance
void studentPerformance(int average)
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

// student roll via recursion
void studentRoll(struct studentData s[], int serialNumber, int n)
{
    if (serialNumber == n)
    {
        return;
    }
    printf("%d ", s[serialNumber].studentRollNumber);
    studentRoll(s, serialNumber + 1, n);
}

int main()
{
    printf("Enter Number of students: ");
    scanf("%d", &n);
    printf("\n\n");
    struct studentData s[n];
    int i = 0;
    while (i < n)
    {
        printf("Enter Roll, name and marks for three subjects for student %d:\n", i + 1);

        if (scanf("%d %s %d %d %d",
                  &s[i].studentRollNumber,
                  s[i].studentName,
                  &s[i].studentMarksSub1,
                  &s[i].studentMakrsSub2,
                  &s[i].studentMarksSub3) != 5) {
            printf("Invalid input! Retry\n");
            while (getchar() != '\n');
            continue;
        }

        if (s[i].studentRollNumber <= 0 ||
            s[i].studentMarksSub1 < 0 || s[i].studentMarksSub1 > 100 ||
            s[i].studentMakrsSub2 < 0 || s[i].studentMakrsSub2 > 100 ||
            s[i].studentMarksSub3 < 0 || s[i].studentMarksSub3 > 100 ||
            !isValidName(s[i].studentName)) {
            printf("Invalid input! Retry\n\n");
            while (getchar() != '\n');
        } else {
            i++;
        }
    }

    printf("\n");
    for (int i = 0; i < n; i++)
    {
        printf("Roll: %d\n", s[i].studentRollNumber);
        printf("Name: %s\n", s[i].studentName);
        printf("Total: %d\n", s[i].studentMarksSub1 + s[i].studentMakrsSub2 + s[i].studentMarksSub3);
        int average = averageMarks(s[i].studentMarksSub1, s[i].studentMakrsSub2, s[i].studentMarksSub3);
        printf("Average: %d\n", average);
        printf("Grade: ");
        studentGrade(average);
        printf("\n");
        if (average >= 35)
        {
            studentPerformance(average);
        }
        printf("\n\n");
    }

    i = 0;
    printf("List of Roll Numbers: ");
    studentRoll(s, i, n);
}