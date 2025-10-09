#include <stdio.h>
#include <stdlib.h>

struct User
{
    int id;
    char name[50];
    int age;
};

void create_file()
{
    FILE *fp = fopen("users.txt", "a");
    if (fp == NULL)
    {
        printf("Error creating file!\n");
        exit(1);
    }
    fclose(fp);
}

void read_name(char name[], int size)
{
    printf("Enter Name: ");
    scanf(" %49[^\n]", name);
}

void create_user()
{
    FILE *fp = fopen("users.txt", "a");
    if (fp == NULL)
    {
        printf("Error opening file!\n");
        return;
    }

    struct User u;
    printf("Enter ID: ");
    scanf("%d", &u.id);

    read_name(u.name, 50);

    printf("Enter Age: ");
    scanf("%d", &u.age);

    fprintf(fp, "%d,%s,%d\n", u.id, u.name, u.age);
    fclose(fp);
    printf("User added.\n");
}

void see_users()
{
    FILE *fp = fopen("users.txt", "r");
    if (fp == NULL)
    {
        printf("No users found.\n");
        return;
    }

    struct User u;
    printf("\nID\tName\tAge\n");
    printf("--------------------------\n");

    while (fscanf(fp, "%d,%49[^,],%d\n", &u.id, u.name, &u.age) != EOF)
    {
        printf("%d\t%s\t%d\n", u.id, u.name, u.age);
    }

    fclose(fp);
}

void user_update()
{
    FILE *fp = fopen("users.txt", "r");
    if (fp == NULL)
    {
        printf("Error opening users.txt\n");
        return;
    }

    FILE *temp = fopen("temp.txt", "w");
    if (temp == NULL)
    {
        printf("Error creating temp.txt\n");
        fclose(fp);
        return;
    }

    struct User u;
    int id, found = 0;
    printf("Enter ID to update: ");
    scanf("%d", &id);

    while (fscanf(fp, "%d,%49[^,],%d\n", &u.id, u.name, &u.age) != EOF)
    {
        if (u.id == id)
        {
            found = 1;
            read_name(u.name, 50);
            printf("Enter new Age: ");
            scanf("%d", &u.age);
        }
        fprintf(temp, "%d,%s,%d\n", u.id, u.name, u.age);
    }

    fclose(fp);
    fclose(temp);

    if (found)
    {
        if (remove("users.txt") != 0 || rename("temp.txt", "users.txt") != 0)
            printf("Error updating file.\n");
        else
            printf("User updated successfully.\n");
    }
    else
    {
        remove("temp.txt");
        printf("User not found.\n");
    }
}

void delete_user()
{
    FILE *fp = fopen("users.txt", "r");
    if (fp == NULL)
    {
        printf("Error opening users.txt\n");
        return;
    }

    FILE *temp = fopen("temp.txt", "w");
    if (temp == NULL)
    {
        printf("Error creating temp.txt\n");
        fclose(fp);
        return;
    }

    struct User u;
    int id, found = 0;
    printf("Enter ID to delete: ");
    scanf("%d", &id);

    while (fscanf(fp, "%d,%49[^,],%d\n", &u.id, u.name, &u.age) != EOF)
    {
        if (u.id == id)
        {
            found = 1;
            continue;
        }
        fprintf(temp, "%d,%s,%d\n", u.id, u.name, u.age);
    }

    fclose(fp);
    fclose(temp);

    if (found)
    {
        if (remove("users.txt") != 0 || rename("temp.txt", "users.txt") != 0)
            printf("Error updating file.\n");
        else
            printf("User deleted successfully.\n");
    }
    else
    {
        remove("temp.txt");
        printf("User not found.\n");
    }
}

int main()
{
    int choice;
    create_file();

    while (1)
    {
        printf("\n--- User Management ---\n");
        printf("1. Add User\n2. Show Users\n3. Update User\n4. Delete User\n5. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
            case 1: create_user(); break;
            case 2: see_users(); break;
            case 3: user_update(); break;
            case 4: delete_user(); break;
            case 5: exit(0);
            default: printf("Invalid choice!\n");
        }
    }

    return 0;
}
