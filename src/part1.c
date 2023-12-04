#include "../include/download.h"

int sockfd;

int createSocket(char *ip, int port){

    struct sockaddr_in server_addr;
    char buf[] = "Mensagem de teste na travessia da pilha TCP/IP\n";
    size_t bytes;
    bzero((char*) &server_addr, sizeof(server_addr));
    server_addr.sin_family= AF_INET;
    server_addr.sin_addr.s_addr= inet_addr(ip);
    server_addr.sin_port= htons(port);

    if ((sockfd = socket(AF_INET, SOCK_STREAM,0 ))< 0 ){
        return -1;
    }
    if (connect(sockfd, (struct sockaddr*)& server_addr, sizeof(server_addr)) < 0)
    {
        return -1;
    };
   // ftp://[<user>:<password>@]<host>/<url-path>
    bytes = write(sockfd, buf ,strlen(buf));
    
    if (bytes > 0){printf("Bytes escritos %ld \n ", bytes);}
    
    else {return -1;}
    
    if (close(sockfd) < 0){return -1;}
    
    return 0;
}
int parseFTP(char *input, struct URL *url) {
    char ftp_regex[] = "ftp://([^:]+):([^@]+)@([^/]+)/(.+)";
    char ftp_generic_regex[] = "ftp://([^/]+)/(.+)";
    regex_t ftp_regex_compiled, ftp_generic_regex_compiled;
    regcomp(&ftp_regex_compiled, ftp_regex, REG_EXTENDED);
    regcomp(&ftp_generic_regex_compiled, ftp_generic_regex, REG_EXTENDED);
    regmatch_t matches[6];
    if (regexec(&ftp_regex_compiled, input, 6, matches, 0) == 0) {
        if (matches[1].rm_so != -1) {
            strncpy(url->user, input + matches[1].rm_so, matches[1].rm_eo - matches[1].rm_so);
            url->user[matches[1].rm_eo - matches[1].rm_so] = '\0';
        }
        if (matches[2].rm_so != -1) {
            strncpy(url->password, input + matches[2].rm_so, matches[2].rm_eo - matches[2].rm_so);
            url->password[matches[2].rm_eo - matches[2].rm_so] = '\0';
        }
        strncpy(url->host, input + matches[3].rm_so, matches[3].rm_eo - matches[3].rm_so);
        url->host[matches[3].rm_eo - matches[3].rm_so] = '\0';
        strncpy(url->resource, input + matches[4].rm_so, matches[4].rm_eo - matches[4].rm_so);
        url->resource[matches[4].rm_eo - matches[4].rm_so] = '\0';
    } else if (regexec(&ftp_generic_regex_compiled, input, 4, matches, 0) == 0) {
        strcpy(url->user, "anonymous");
        strcpy(url->password, "password");
        strncpy(url->host, input + matches[1].rm_so, matches[1].rm_eo - matches[1].rm_so);
        url->host[matches[1].rm_eo - matches[1].rm_so] = '\0';
        strncpy(url->resource, input + matches[2].rm_so, matches[2].rm_eo - matches[2].rm_so);
        url->resource[matches[2].rm_eo - matches[2].rm_so] = '\0';
    }
    else {
        regfree(&ftp_regex_compiled);
        regfree(&ftp_generic_regex_compiled);
        return -1;  
    }
    struct hostent *h;
    if ((h = gethostbyname(url->host)) == NULL) {
        herror("gethostbyname()");
        return -1;
    }
    strcpy(url->ip, inet_ntoa(*((struct in_addr *) h->h_addr)));
    regfree(&ftp_regex_compiled);
    regfree(&ftp_generic_regex_compiled);
    return 0;
}


int passive_mode(const int socket ,char *ip, int *port){
    char answer[MAX_LENGTH];
    int ip1, ip2, ip3, ip4, port1, port2;
    if (readResponse(socket, answer) != RESPONSE_CODE_PASSIVE)
    {
        return -1;
    }
    sscanf(answer, PASSIVE_REGEX, &ip1, &ip2, &ip3, &ip4, &port1, &port2);
    *port = port1 * 256 + port2;
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


int requestResource(int socket, char *resource){
    char buf[MAX_LENGTH];
    char resourceCommand[5+strlen(resource)+1];
    sprintf(resourceCommand, "retr %s\n", resource);
    write(socket, resourceCommand, strlen(resourceCommand));
    return readResponse(socket,buf);
}

int getResource(int socketA, int socketB, char *resource){
    FILE *fd = fopen(resource, "wb");
    if(fd==NULL){exit(-1);}
    char buf[MAX_LENGTH];
    int bytes;
    while((bytes=read(socketB, buf, MAX_LENGTH))>0){
        if(fwrite(buf, 1, bytes, fd)==0){return -1;}
    }
    fclose(fd);
    return readResponse(socketA,buf);
}

int close_connection(const int socketA, const int socketB){
    char answer[MAX_LENGTH];
    write(socketA, "quit\n", 5);
    if(readResponse(socketA,answer)!=221){return -1;} //221 Service closing control connection.
    memset(answer, 0, MAX_LENGTH);
    write(socketB, "quit\n", 5);
    if(readResponse(socketB,answer)!=221){return -1;} //221 Service closing control connection.
    return 0;
}
