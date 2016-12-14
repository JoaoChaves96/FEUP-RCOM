#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <netdb.h>

#include "get_url.h"

/*
* Initializes the url_info struct with FEUP's default username("anonymus") and password("mail@domain")
*/
void init_default(url_info * i){
  int user_size = strlen(DEF_USERNAME) + 1;
  int password_size = strlen(DEF_PASSWORD) + 1;
  memcpy(i->username, DEF_USERNAME, user_size);
  memcpy(i->password, DEF_PASSWORD, password_size);
};

/*
* Initializes the url_info struct with the given values
*/
int init_authorized(url_info * i, char* url){
  char header[] = "ftp://";
  char input_header[6];
  memcpy(input_header, url, 6);

  if(strncmp(header, url, 6) != 0)
{
  printf("Incorrect header of download url, exiting...\n");
  return 1;
}

  url += 6;

  char * end_user = strchr(url, ':'); // finds the beggining
  char * password = end_user + 1;             // of the password

  if(end_user - url == 0){
    printf("Url must contain a usernanme, exiting...\n");
    return 1;
  }
  else
    memcpy(i->username, url, end_user - url);

  char * end_password = strrchr(url, '@');

  if(end_password - password == 0){
    printf("Url must contain a password, exiting...\n");
    return 1;
  }
  else
    memcpy(i->password, password, end_password - password);

  char * host = end_password + 1;

  char * end_host = strchr(url, '/');

  memcpy(i->host, host, end_host - host);

  char * path = end_host;

  char * end_path = strrchr(url, '/') + 1;

  memcpy(i->path, path, end_path - path);

  char * end_file = end_path;

  if(url - end_file == 0){
    printf("Url must contain a valid file, exiting...\n");
    return 1;
  }
  else
    memcpy(i->file_name, end_file, strlen(end_file));

  if(i->host_info=gethostbyname(i->host) == NULL)

  printf("Username: %s\nPassword: %s\nHost: %s\nPath: %s\nFile:%s\n", i->username, i->password, i->host, i->path, i->file_name);

  return 0;
}

int get_url(char * url, url_info * info){
    memset(info->username, 0, 256);
    memset(info->password, 0, 256);
    memset(info->path, 0, 256);
    memset(info->file_name, 0, 256);

  if(strchr(url, '@') == NULL){
    init_default(info);
    return 0;
  }

  if(init_authorized(info, url) != 0)
    return 1;

  return 0;
}
