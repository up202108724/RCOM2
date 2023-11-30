#include "download.h"

int sockfd;

int createSocket(char *ip, int port){

    struct sockaddr_in server_addr;
    char buf[] = "Mensagem de teste na travessia da pilha TCP/IP\n";
    size_t bytes;
    bzero((char*) &server_addr, sizeof(server_addr));
    server_addr.sin_family= AF_INET;
    server_addr.sin_addr.s_addr= inet_addr(*ip);
    server_addr.sin_port= htons(port);

    if ((sockfd = socket(AF_INET, SOCK_STREAM,0 ))< 0 ){
        return -1;
    }
    if (connect(sockfd, (struct sockaddr*)& server_addr, sizeof(server_addr)) < 0)
    {
        return -1;
    };
    ftp://[<user>:<password>@]<host>/<url-path>
    bytes = write(sockfd, buf ,strlen(buf));
    
    if (bytes > 0){printf("Bytes escritos %ld \n ", bytes);}
    
    else {return -1;}
    
    if (close(sockfd) < 0){return -1;}
    
    return 0;
}

int parseFTP(char *input, struct URL *url)
{
    //ftp://[<user>:<password>@]<host>/<url-path>
    regex_t re;
    regcomp(&re, "/" , 0);
    if(regexec(&re, input , 0, NULL ,0 )!=0){ return -1;}
    regcomp(&re, "@", 0 );
    if(regexec(&re,input , 0, NULL, 0)!=0){ //ftp://<host>/<url-path>
        sscanf(input, HOST_REGEX, url->host);
        strcpy(url->user, "anonymous");
        strcpy(url->password, "password");
    }
    else{
        sscanf(input, HOST_AT_REGEX, *url->host);
        sscanf(input, USER_REGEX, *url->user);
        sscanf(input, PASS_REGEX, *url->password);

    }
    struct hostent *h;
    if(strlen(url->host)==0){return -1;}
    if ((h = gethostbyname(url->user)) == NULL) {
        return -1;
    }

    printf("Host name  : %s\n", h->h_name);
    printf("IP Address : %s\n", inet_ntoa(*((struct in_addr *) h->h_addr)));

}
int passive_mode(const int socket ,char *ip, int port){
    char answer[MAX_LENGTH];
    int ip1, ip2, ip3, ip4, port1, port2;
    if (readResponse(socket, answer) != RESPONSE_CODE_PASSIVE)
    {
        return -1;
    }
    sscanf(answer, PASSIVE_REGEX, &ip1, &ip2, &ip3, &ip4, &port1, &port2);
    port = port1 * 256 + port2;
    sprintf(ip, "%d.%d.%d.%d", ip1, ip2, ip3, ip4);
    return RESPONSE_CODE_PASSIVE;
}
int readResponse(int socket, char *buf){
    char byte;
    int i = 0;
    int responseCode;
    ResponseState state = START;
    memset(buf, 0, MAX_LENGTH);
    while (state != END)
    {
        if (read(socket, &byte, 1) < 0)
        {
            return -1;
        }
        switch (state)
        {
        case START:
            if (byte == ' ')
            {
                state = SINGLE;
            }
            else if (byte == '-')
            {
                state = MULTIPLE;
            }
           else if (byte == '\n')
            {
                state = END;
            }
            else
            {
                buf[i++] = byte;
            }
            break;
        case SINGLE:
            if (byte == '\n')
            {
                state = END;
            }
            else
            {
                buf[i++] = byte;
            }
            break;
        case MULTIPLE:
            if (byte == '\n')
            {
                state = START;
                i = 0;
                memset(buf, 0, MAX_LENGTH);

            }
            else
            {
                buf[i++] = byte;
            }
            break;
        case END:
            break;
        default:
            break;
        }
    }
    sscanf(buf, "%d", &responseCode);
    return responseCode;
}

int authenticate(int socket, char *user, char *password){
    char buf[MAX_LENGTH];
    char userCommand[5+strlen(user)+1];
    char passCommand[5+strlen(password)+1];
    sprintf(userCommand, "user %s\n", user);
    sprintf(passCommand, "pass %s\n", password);
    
    write(socket, userCommand, strlen(userCommand));
    if(readResponse(socket,buf)!=331){exit(-1);} //331 User name okay, need password.
    memset(buf, 0, MAX_LENGTH);
    write(socket, passCommand, strlen(passCommand));
    return readResponse(socket,buf); //230 User logged in
}

