#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

pthread_mutex_t mutex;

void *handleClient(void *arg) {
    int clientSock = *(int *)arg;
    free(arg);

    int choice, amount, balance;
    FILE *fp;

    read(clientSock, &choice, sizeof(int));

    pthread_mutex_lock(&mutex);

    fp = fopen("accountDB.txt", "r+");
    fscanf(fp, "%d", &balance);

    if (choice == 1) {
        read(clientSock, &amount, sizeof(int));
        if (amount <= balance) {
            balance -= amount;
            rewind(fp);
            fprintf(fp, "%d", balance);
            write(clientSock, &balance, sizeof(int));
        } else {
            int fail = -1;
            write(clientSock, &fail, sizeof(int));
        }
    }
    else if (choice == 2) {
        read(clientSock, &amount, sizeof(int));
        balance += amount;
        rewind(fp);
        fprintf(fp, "%d", balance);
        write(clientSock, &balance, sizeof(int));
    }
    else if (choice == 3) {
        write(clientSock, &balance, sizeof(int));
    }

    fclose(fp);
    pthread_mutex_unlock(&mutex);

    close(clientSock);
    return NULL;
}

int main() {
    int serverSock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    bind(serverSock, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    listen(serverSock, 5);

    pthread_mutex_init(&mutex, NULL);

    printf("ATM Server running...\n");

    while (1) {
        int clientSock = accept(serverSock, NULL, NULL);
        int *p = malloc(sizeof(int));
        *p = clientSock;

        pthread_t tid;
        pthread_create(&tid, NULL, handleClient, p);
        pthread_detach(tid);
    }
    return 0;
}
