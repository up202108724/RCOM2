#include "download.h"
#include <pcre.h>
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

int parse_ftp_url(const char *url_str, struct URL *url) {
    const char *pattern = "(?i)ftp://(?:([^\\s:]+)(?::([^\\s@]+))?@)?([^/]+)(?:/([^\\s]+))?";
    const char url_str;
    int rc;
    const int max_groups = 4;
    int ovector[3 * (max_groups + 1)];  // (max_groups+1) * 3

    pcre *re;
    const char *error;
    int erroffset;

    re = pcre_compile(pattern, 0, &error, &erroffset, NULL);
    if (re == NULL) {
        fprintf(stderr, "Error in PCRE compilation at offset %d: %s\n", erroffset, error);
        return;
    }

    rc = pcre_exec(re, NULL, url_str, strlen(url_str), 0, 0, ovector, sizeof(ovector) / sizeof(ovector[0]));

    if (rc < 0) {
        fprintf(stderr, "Error in PCRE execution: %d\n", rc);
        pcre_free(re);
        return;
    }

    if (rc > 0) {
        int i;
        for (i = 0; i < rc; ++i) {
            int start = ovector[2 * i];
            int end = ovector[2 * i + 1];

            if (i == 1) {
                if (start >= 0) {
                    strncpy(url->user, url_str + start, end - start);
                    url->user[end - start] = '\0';
                } else {
                    // User not provided, set default
                    strcpy(url->user, "anonymous");
                }
            } else if (i == 2) {
                if (start >= 0) {
                    strncpy(url->password, url_str + start, end - start);
                    url->password[end - start] = '\0';
                } else {
                    // Password not provided, set default
                    strcpy(url->password, "password");
                }
            } else if (i == 3) {
                strncpy(url->host, url_str + start, end - start);
                sscanf(url->host, "%*d.%*d.%*d.%s", url->ip);
            } else if (i == 4) {
                if (start >= 0) {
                    strncpy(url->url_path, url_str + start, end - start);
                    url->url_path[end - start] = '\0';
                } else {
                    // URL path not provided
                    strcpy(url->url_path, "");
                }
            }
        }
    

    pcre_free(re);
    struct hostent *h;
    if(strlen(url->host)==0){return -1;}
    if ((h = gethostbyname(url->user)) == NULL) {
        return -1;
    }

    printf("Host name  : %s\n", h->h_name);
    printf("IP Address : %s\n", inet_ntoa(*((struct in_addr *) h->h_addr)));

}
 //ftp://[<user>:<password>@]<host>/<url-path>
    /*
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
    */
