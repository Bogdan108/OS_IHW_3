#include "GeneralData.h"

#define HISTORYBUFFER 10000
#define MAXPENDING 30 /* Maximum outstanding connection requests */
int servSock;         /* Socket descriptor for server */
int clntSock;         /* Socket descriptor for client */

void my_handler(int nsig)
{
    close(clntSock);
    exit(0);
}

int cmp(const void *a, const void *b)
{
    return *(int *)a - *(int *)b;
}

// структура сохраненного запроса
struct request
{
    int client_type;
    int client_index;
    int client_value;
};

int main(int argc, char *argv[])
{
    struct sockaddr_in echoServAddr; /* Local address */
    struct sockaddr_in echoClntAddr; /* Client address */
    unsigned short echoServPort;     /* Server port */
    unsigned int clntLen;            /* Length of client address data structure */
    int history_iterator = 0;

    // проверка входных данных
    if (argc != 3)
    {
        fprintf(stderr, "Usage:  %s <Server Port>\n", argv[0]);
        exit(1);
    }

    struct request history[HISTORYBUFFER];
    int request[5];
    int answer[5];
    int recvMsgSize;

    // инициализация базы данных и заполнение ее данными
    int dataBase[BUFFER_SIZE];

    for (int i = 0; i < BUFFER_SIZE; ++i)
    {
        dataBase[i] = i;
    }

    echoServPort = atoi(argv[2]); /* Second arg:  local port */

    /* Create socket for incoming connections */
    if ((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        perror("socket() failed");
        exit(1);
    }

    (void)signal(SIGINT, my_handler);
    /* Construct local address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));   /* Zero out structure */
    echoServAddr.sin_family = AF_INET;                /* Internet address family */
    echoServAddr.sin_addr.s_addr = inet_addr(argv[1]); /* Any incoming interface */
    echoServAddr.sin_port = htons(echoServPort);      /* Local port */

    /* Bind to the local address */
    if (bind(servSock, (struct sockaddr *)&echoServAddr, sizeof(echoServAddr)) < 0)
    {
        perror("bind() failed");
        exit(1);
    }

    printf("Server IP address = %s. Wait...\n", inet_ntoa(echoClntAddr.sin_addr));

    /* Mark the socket so it will listen for incoming connections */
    if (listen(servSock, MAXPENDING) < 0)
    {
        perror("listen() failed");
        exit(1);
    }

    for (;;) /* Run forever */
    {
        /* Set the size of the in-out parameter */
        clntLen = sizeof(echoClntAddr);

        /* Wait for a client to connect */
        if ((clntSock = accept(servSock, (struct sockaddr *)&echoClntAddr,
                               &clntLen)) < 0)
        {
            perror("accept() failed");
            exit(1);
        }

        /* clntSock is connected to a client! */

        printf("Handling client %s\n", inet_ntoa(echoClntAddr.sin_addr));

        int recvMsgSize; /* Size of received message */

        /* Receive message from client */
        if ((recvMsgSize = recv(clntSock, request, sizeof(request), 0)) < 0)
        {
            perror("recv() failed");
            exit(1);
        }

        int index = request[1];
        int data = request[2];

        // обработка клиента писателя
        if (request[0] == WRITER)
        {
            dataBase[index] = data;
            printf("Сlient writer wanted to write to a cell with an index %d number: %d\n", index, data);
            qsort(dataBase, BUFFER_SIZE, sizeof(int), cmp);
            answer[0] = request[0];
            answer[1] = request[1];
            answer[2] = dataBase[index];
            if (history_iterator < HISTORYBUFFER)
            {
                history[history_iterator].client_type = answer[0];
                history[history_iterator].client_index = answer[1];
                history[history_iterator].client_value = answer[2];
                history_iterator++;
            }
            else
            {
                printf("Server can't save history \n");
            }
        }
         // обработка клиента читателя
        if (request[0] == READER)
        {
            printf("Сlient read wanted to read to a cell with an index %d number: %d\n", index, dataBase[index]);
            answer[0] = request[0];
            answer[1] = request[1];
            answer[2] = dataBase[index];
            if (history_iterator < HISTORYBUFFER)
            {
                history[history_iterator].client_type = answer[0];
                history[history_iterator].client_index = answer[1];
                history[history_iterator].client_value = answer[2];
                history_iterator++;
            }
            else
            {
                printf("Server can't save history \n");
            }
        }

         // обработка клиента наблюдателя
        if (request[0] == VISITOR)
        {   
            if(request[3] < HISTORYBUFFER && request[3] <= history_iterator){
                // передаю сохраненные данные
                answer[0] = history[request[3]].client_type;
                answer[1] = history[request[3]].client_index;
                answer[2] = history[request[3]].client_value;
                answer[4] = 1;
                // обновляю итератор клиента
                answer[3] = request[3] + 1;
            }
            else{
                printf("Can't send new information to visitor\n");
                answer[4] = 0;
            }
            printf("Send to client visitor information\n");
        }

        /* Send received data and receive again until end of transmission */
        while (recvMsgSize > 0) /* zero indicates end of transmission */
        {
            /* Echo message back to client */
            if (send(clntSock, answer, recvMsgSize, 0) != recvMsgSize)
            {
                perror("send() failed");
                exit(1);
            }

            /* See if there is more data to receive */
            if ((recvMsgSize = recv(clntSock, request, sizeof(request), 0)) < 0)
            {
                perror("recv() failed");
                exit(1);
            }
        }

        close(clntSock); /* Close client socket */
    }
}
