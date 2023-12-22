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

/* Parser output */

struct URL{
    char host[MAX_LENGTH];
    char user[MAX_LENGTH];
    char password[MAX_LENGTH];
    char ip[MAX_LENGTH];
    char resource[MAX_LENGTH];
    char filename[MAX_LENGTH]; 
};

/**
 * @brief Creates a socket connection.
 * 
 * @param ip The IP address to connect to.
 * @param port The port number to connect to.
 * @return int Returns the socket descriptor on success.
 */

int createSocket(char *ip, int port);

/**
 * @brief Parses FTP input and extracts URL information.
 * 
 * @param input The input string containing FTP URL.
 * @param url A struct to store parsed URL components.
 * @return int Returns 0 on success, -1 on failure.
 */

int parseFTP(char* input , struct URL *url);

/**
 * @brief Reads the server's response from a socket.
 * 
 * @param socket The socket descriptor.
 * @param buf A buffer to store the response.
 * @return int Returns the response code.
 */

int readResponse(int socket, char *buf);

/**
 * @brief Authenticates with the FTP server using a username and password.
 * 
 * @param socket The socket descriptor.
 * @param user The username for authentication.
 * @param password The password for authentication.
 * @return int Returns the response code.
 */

int authenticate(int socket, char *user, char *password);

/**
 * @brief Enters passive mode for data transfer.
 * 
 * @param socket The control socket descriptor.
 * @param ip A buffer to store the IP address for passive mode.
 * @param port A pointer to an integer to store the port for passive mode.
 * @return int Returns 0 on success, -1 on failure.
 */


int passive_mode(const int socket ,char *ip, int *port);

/**
 * @brief Requests a specific resource from the FTP server.
 * 
 * @param socket The control socket descriptor.
 * @param resource The resource to request.
 * @return int Returns the response code.
 */

int requestResource(int socket, char *resource);

/**
 * @brief Retrieves a resource from the FTP server.
 * 
 * @param socketA The control socket descriptor.
 * @param socketB The data socket descriptor for data transfer.
 * @param resource The resource to retrieve.
 * @return int Returns the response code.
 */

int getResource(int socketA, int socketB, char *resource);

/**
 * @brief Closes the FTP connection by closing both sockets.
 * 
 * @param socketA The control socket descriptor.
 * @param socketB The data socket descriptor for data transfer.
 * 
 * @return int Returns 0 on successful closure of both sockets, -1 on failure.
 */

int close_connection(int socketA, int socketB);