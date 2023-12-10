#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <regex.h>
#include <string.h>

#define BAR "/"
#define MAX_LENGTH 500
#define MAX_LENGTH_BUFFER 1024

#define PASSIVE_REGEX   "%*[^(](%d,%d,%d,%d,%d,%d)%*[^\n$)]"

#define RC_PASSIVE 227
#define RC_FILESTATUSOK 150
#define RC_CLOSINGDATACONNECTION 226
#define RC_USEROK 331
#define RC_LOGGED_IN 230
#define RC_AUTH_SUCCESSFUL 230
#define RC_SERVICE_READY 220
#define SERVICE_CLOSING_CONTROL_CONNECTION 221

struct URL{
    char host[MAX_LENGTH];
    char user[MAX_LENGTH];
    char password[MAX_LENGTH];
    char ip[MAX_LENGTH];
    char resource[MAX_LENGTH];
    char filename[MAX_LENGTH]; 
};

typedef enum {
    START,
    SINGLE,
    MULTIPLE,
    END
} ResponseState;


int createSocket(char *ip, int port);

int parseFTP(char* input , struct URL *url);

int readResponse(int socket, char *buf);

int authenticate(int socket, char *user, char *password);

int passive_mode(const int socket ,char *ip, int *port);

int requestResource(int socket, char *resource);

int getResource(int socketA, int socketB, char *resource);

int close_connection(int socketA, int socketB);