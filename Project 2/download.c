#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>

#include "get_url.h"

int main(int arg, char ** argv){

  if(argc != 2){
    printf("Invalid number of arguments, exiting...\n");
    exit(1);
  }

  url_info info;

  if(get_url(agrv[1], &info)){
    printf("\n Invalid URL, exiting...\n");
    exit(1);
  }

  
}
