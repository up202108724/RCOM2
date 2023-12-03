#include "download.h"

int main(int argc, int argv[]){
    
    if(argc!=2){
        printf("Usage: ./download ftp://[<user>:<password>@]<host>/<url-path>\n");
        return -1;
    }
    struct URL url;
    memset(&url, 0, sizeof(url));
    if (parseFTP(argv[1], &url)!= 0 ){
        return -1;
    }
    char answer[MAX_LENGTH];
    int socketA = createSocket(url.ip, 21); //21 is the default port for FTP
    if (socketA < 0 || readResponse(socket, answer) != 220){ // 220 is the response code for connection established
        return -1;
    }
    if (authenticate(socketA, url.user, url.password) != 230){ // 230 is the response code for authentication successful
        printf("Authentication failed, user and password not matching\n");
        return -1;
    }
    int socketB = createSocket(url.ip, 21);
    if (socketA < 0 || readResponse(socket, answer) != 220){ // 220 is the response code for connection established
        return -1;
    }
    int port;
    char ip[MAX_LENGTH];
    if (passive_mode(socket, ip, port) != RESPONSE_CODE_PASSIVE){
        return -1;
    }

    if (close_connection(socketA, socketB)){
        return -1;
    }
}