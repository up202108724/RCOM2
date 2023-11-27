#include <download.h>
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

