#ifndef _GET_URL_H
#define _GET_URL_H

#define DEF_USERNAME "anonymus"
#define DEF_PASSWORD "mail@domain"

typedef struct{
    char username[256];
    char password[256];
    char host[256];
    char path[256];
    char file_name[256];
    struct hostent * host_info;
} url_info;

int get_url(char* url, url_info * info);

#endif
