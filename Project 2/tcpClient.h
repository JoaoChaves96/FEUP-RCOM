#include "get_url.h"

#define SERVER_PORT 21

int open_connection(char * addr, int port);

void login(int control_socket, url_info * i);

void passive_mode(int control_socket, char* address, int * port);

void retreive(int control_socket, url_info * i);

int download(int data_socket, url_info * i);

int close_connection(int control_socket, int data_socket);
