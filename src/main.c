#include "download.h"

int main(int argc, char *argv[]){
    
    if(argc!=2){
        printf("Usage: ./download ftp://[<user>:<password>@]<host>/<url-path>\n");
        return -1;
    }
    struct URL url;
    memset(&url, 0, sizeof(url));
    if (parseFTP(argv[1], &url)!= 0 ){
        return -1;
    }
    struct hostent *h;
    if ((h = gethostbyname(url.host)) == NULL) {
        herror("gethostbyname()");
        exit(-1);
    }
    char answer[MAX_LENGTH];
    int socketA = createSocket(url.ip, 21); //21 is the default port for FTP
    if (socketA < 0 || readResponse(socketA, answer) != 220){ // 220 is the response code for connection established
        exit(-1);
    }
    if (authenticate(socketA, url.user, url.password) != 230){ // 230 is the response code for authentication successful
        printf("Authentication failed, user and password not matching\n");
        exit(-1);
    }
    int socketB = createSocket(url.ip, 21);
    if (socketB < 0 || readResponse(socketB, answer) != 220){ // 220 is the response code for connection established
        exit(-1);
    }
    int *port;
    char ip[MAX_LENGTH];
    if (passive_mode(socketA, ip, *port) != RESPONSE_CODE_PASSIVE){
        exit(-1);
    }

    if(requestResource(socketA, url.resource)!=RESPONSE_CODE_READY_FOR_TRANSFER){
        printf("Error requesting resource\n");
        exit(-1);
    }

    if(getResource(socketA, socketB, url.resource)!=RESPONSE_CODE_TRANSFER_COMPLETE){
        printf("Error getting resource\n");
        exit(-1);
    }

    if (close_connection(socketA, socketB)){
        exit(-1);
    }
}