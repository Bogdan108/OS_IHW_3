#include "GeneralData.h"

int sock; /* Socket descriptor */
void my_handler(int nsig)
{
    close(sock);
    exit(0);
}

int main(int argc, char *argv[])
{
    struct sockaddr_in echoServAddr; /* Echo server address */
    unsigned short echoServPort;     /* Echo server port */
    char *servIP;                    /* Server IP address (dotted quad) */
    unsigned int echoLen;            /* Length of string to echo */
    int bytesRcvd, totalBytesRcvd;   /* Bytes read in single recv()
                                        and total bytes read */
    int number_of_readers;

    if (argc != 4) /* Test for correct number of arguments */
    {
        fprintf(stderr, "Usage: %s <Server IP> [<Echo Port>] [<number_of_readers>]\n",
                argv[0]);
        exit(1);
    }

    // обработка сигнала прерывания
    (void)signal(SIGINT, my_handler);

    servIP = argv[1];                  /* First arg: server IP address (dotted quad) */
    echoServPort = atoi(argv[2]);      /* Use given port, if any */
    number_of_readers = atoi(argv[3]); /* Second arg: string to echo */

    srand(0);
    for (int i = 0; i < number_of_readers; ++i)
    {
        /* Create a reliable, stream socket using TCP */
        if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        {
            perror("socket() failed");
            exit(1);
        }

        /* Construct the server address structure */
        memset(&echoServAddr, 0, sizeof(echoServAddr));   /* Zero out structure */
        echoServAddr.sin_family = AF_INET;                /* Internet address family */
        echoServAddr.sin_addr.s_addr = inet_addr(servIP); /* Server IP address */
        echoServAddr.sin_port = htons(echoServPort);      /* Server port */

        /* Establish the connection to the echo server */
        if (connect(sock, (struct sockaddr *)&echoServAddr, sizeof(echoServAddr)) < 0)
        {
            perror("connect() failed");
            exit(1);
        }

        int numbers[5];
        echoLen = sizeof(numbers); /* Determine input length */

        int index = rand() % BUFFER_SIZE;
        int data = rand() % BUFFER_SIZE;

        numbers[0] = READER;
        numbers[1] = index;
        numbers[2] = data;

        if (send(sock, numbers, echoLen, 0) != echoLen)
        {
            perror("send() sent a different number of bytes than expected");
            exit(1);
        }

        /* Receive the same string back from the server */
        totalBytesRcvd = 0;
        printf("%d) Received: ", i); // Setup to print the echoed string

        // массив для получения результата ответа сервера
        int result[5];
        while (totalBytesRcvd < echoLen)
        {
            /* Receive up to the buffer size (minus 1 to leave space for
            a null terminator) bytes from the sender */
            if ((bytesRcvd = recv(sock, result, echoLen, 0)) <= 0)
            {
                perror("recv() failed or connection closed prematurely");
                exit(1);
            }
            totalBytesRcvd += bytesRcvd; /* Keep tally of total bytes */
        }
        printf("Reader: Process %d successfully read data from index %d: %d\n", i, result[1], result[2]);
        printf("\n"); /* Print a final linefeed */
        sleep(1);
        close(sock);
    }
    return (0);
}
