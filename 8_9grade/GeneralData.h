#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

// размер исходной базы данных
#define BUFFER_SIZE 150

// коды типов клиентов
#define READER  0     
#define WRITER 1
#define VISITOR 2

// структура сообщения, передаваемого от сервера к клиенту
typedef struct {
    int index;
    int data;
} serverAnswer;

// структура сообщения, передаваемого от клиента к серверу
typedef struct {
    int client_type;
    int index;
    int data;
} clientRequest;