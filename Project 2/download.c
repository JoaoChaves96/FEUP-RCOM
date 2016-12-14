#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>

#include "get_url.h"
#include "tcpClient.h"

int main(int argc, char ** argv){

  if(argc != 2){
    printf("Invalid number of arguments, exiting...\n");
    exit(1);
  }

  url_info info;

  if(get_url(argv[1], &info)){
    printf("\n Invalid URL, exiting...\n");
    exit(1);
  }

  int control_socket;

  if((control_socket = open_connection(inet_ntoa(*((struct in_addr *)info.host_info->h_addr)), SERVER_PORT)) == 0){
    printf("Error setting control connection");
    exit(1);
  }

  login(control_socket, &info);
  char address[512];
  int port;
  passive_mode(control_socket, address, &port);

  int data_socket;
  if((data_socket=open_connection(address, port)) == 0){
    printf("Error opening data socket, exiting...\n");
    exit(1);
  }

  retreive(control_socket, &info);
  download(data_socket, &info);
  close_connection(control_socket, data_socket);

  return 0;
}
