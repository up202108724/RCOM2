#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <regex.h>
#include <string.h>

#define BAR "/"
#define MAX_LENGTH 500

#define HOST_REGEX "%*[^/]//%[^/]"



struct URL{
    char host[MAX_LENGTH];
    char user[MAX_LENGTH];
    char password[MAX_LENGTH];
    char ip[MAX_LENGTH];
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