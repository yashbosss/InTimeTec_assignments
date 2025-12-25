#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int choice, amount, result;

    while (1) {
        int sock = socket(AF_INET, SOCK_STREAM, 0);

        struct sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(8080);
        serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

        connect(sock, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

        printf("\n1. Withdraw\n");
        printf("2. Deposit\n");
        printf("3. Display Balance\n");
        printf("4. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        write(sock, &choice, sizeof(int));

        if (choice == 1 || choice == 2) {
            printf("Enter amount: ");
            scanf("%d", &amount);
            write(sock, &amount, sizeof(int));
        }

        if (choice == 4) {
            close(sock);
            break;
        }

        read(sock, &result, sizeof(int));

        if (result == -1)
            printf("Insufficient balance\n");
        else
            printf("Current balance: %d\n", result);

        close(sock);
    }

    return 0;
}
