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
    int i = 0;
    char ch;
    printf("Enter Name: ");
    while ((ch = getchar()) != '\n' && i < size - 1)
    {
        name[i++] = ch;
    }
    name[i] = '\0';
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
    getchar();

    read_name(u.name, 50);

    printf("Enter Age: ");
    scanf("%d", &u.age);
    getchar();

    fprintf(fp, "%d,%s,%d\n", u.id, u.name, u.age);
    fclose(fp);
    printf("User added.\n");
}

void see_users() {
    FILE *fp = fopen("users.txt", "r");
    struct User u;

    if (fp == NULL) {
        printf("No users found.\n");
        return;
    }

    printf("\nID\tName\tAge\n");
    printf("--------------------------\n");

    while (fscanf(fp, "%d,%49[^,],%d\n", &u.id, u.name, &u.age) != EOF) {
        printf("%d\t%s\t%d\n", u.id, u.name, u.age);
    }

    fclose(fp);
}


void user_update()
{
    FILE *fp = fopen("users.txt", "r");
    FILE *temp = fopen("temp.txt", "w");
    struct User u;
    int id, found = 0;

    if (fp == NULL || temp == NULL)
    {
        printf("Error opening file.\n");
        return;
    }

    printf("Enter ID to update: ");
    scanf("%d", &id);
    getchar();

    while (fscanf(fp, "%d,%49[^,],%d\n", &u.id, u.name, &u.age) != EOF)
    {
        if (u.id == id)
        {
            found = 1;
            read_name(u.name, 50);
            printf("Enter new Age: ");
            scanf("%d", &u.age);
            getchar();
        }
        fprintf(temp, "%d,%s,%d\n", u.id, u.name, u.age);
    }

    fclose(fp);
    fclose(temp);

    if (found)
    {
        remove("users.txt");
        rename("temp.txt", "users.txt");
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
    FILE *temp = fopen("temp.txt", "w");
    struct User u;
    int id, found = 0;

    if (fp == NULL || temp == NULL)
    {
        printf("Error opening file.\n");
        return;
    }

    printf("Enter ID to delete: ");
    scanf("%d", &id);
    getchar();

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
        remove("users.txt");
        rename("temp.txt", "users.txt");
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
        printf("1. Add User\n");
        printf("2. Show Users\n");
        printf("3. Update User\n");
        printf("4. Delete User\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar();

        if (choice == 1)
            create_user();
        else if (choice == 2)
            see_users();
        else if (choice == 3)
            user_update();
        else if (choice == 4)
            delete_user();
        else if (choice == 5)
            break;
        else
            printf("Invalid choice!\n");
    }

    return 0;
}
