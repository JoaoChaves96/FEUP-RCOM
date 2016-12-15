#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>

#include "tcpClient.h"

#define TYPE_READ 1
#define TYPE_NO_READ 0

int open_connection(char * addr, int port){
  int	sockfd;
	struct	sockaddr_in server_addr;

	/*server address handling*/
	bzero((char*)&server_addr,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(addr);	/*32 bit Internet address network byte ordered*/
	server_addr.sin_port = htons(port);		/*server TCP port must be network byte ordered */

	/*open an TCP socket*/
	if ((sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0) {
    		perror("socket()");
        	return 0;
    	}
	/*connect to the server*/
    	if(connect(sockfd,
	           (struct sockaddr *)&server_addr,
		   sizeof(server_addr)) < 0){
        	perror("connect()");
		return 0;
	}

  return sockfd;
}

int read_s(int control_socket, char* res){
  FILE * f = fdopen(control_socket, "r");

  int mem_alloc = 0;

  if(res == NULL){
    res = (char *) malloc(sizeof(char) * 512);
    mem_alloc = 1;
  }

  do{
    memset(res, 0, 512);
    res = fgets(res, 512, f);
    printf("%s\n", res);
  } while (!('1' <= res[0] && res[0] <= '5') || res[3] != ' ');

  char rep = res[0];

  if(mem_alloc)
    free(res);

  return (rep > '4');
}

int write_s(int control_socket, char* code, char* res, int read){
  int ret = write(control_socket, code, strlen(code));

  if(read){
    return read_s(control_socket, res);
  }
  else
    return (ret == 0);
}

void login(int control_socket, url_info * i){
  char username[512];
  char password[512];

  read_s(control_socket, NULL);

  sprintf(username, "USER %s\r\n", i->username);
  write_s(control_socket, username, NULL, TYPE_READ);
  printf("%s\n", username);
  sprintf(password, "PASS %s\r\n", i->password);
  printf("%s\n", password);
  if(write_s(control_socket, password, NULL, TYPE_READ) != 0){
    fprintf(stderr, "Bad login. Exiting...\n");
    exit(1);
  }
}


void passive_mode(int control_socket, char* address, int * port){
  char res[512];

  if(write_s(control_socket, "PASV\r\n", res, TYPE_READ) != 0){
    printf("Error entering in passive mode, exiting...\n");
    exit(1);
  }

  int values[6];

  char * data = strchr(res, '(');
  sscanf(data, "(%d, %d, %d, %d, %d, %d)", &values[0],&values[1],&values[2],&values[3],&values[4],&values[5]);
  sprintf(address, "%d.%d.%d.%d", values[0],values[1],values[2],values[3]);

  *port = values[4]*256 + values[5];
}

void retreive(int control_socket, url_info * i){
  char res[512];

  write_s(control_socket, "TYPE L 8\r\n", NULL, TYPE_READ);
  sprintf(res, "RETR %s%s\r\n", i->path, i->file_name);

  if(write_s(control_socket, res, NULL, TYPE_READ) != 0){
    printf("Error on retreiving the file, exiting...\n");
    exit(1);
  }
}

int download(int data_socket, url_info * i){
  FILE * output;
  if(!(output = fopen(i->file_name, "w"))){
    printf("Error, cannot open file, exiting...\n");
    exit(1);
  }

  char buf[1024];
  int bytes;
  printf("Started downloading the file...\n");
  while((bytes=read(data_socket, buf, sizeof(buf)))){
    if (bytes < 0){
      printf("Cannot receive anything from data_socket, exiting...\n");
      return 1;
    }

    if ((bytes=fwrite(buf, bytes, 1, output)) < 0){
      printf("Cannot write data in the file, exiting...\n");
      return 1;
    }
    printf("Receiving data...\n");
  }

  fclose(output);

  printf("Finished downloading th file sucessfully!\n");

  return 0;
}

int close_connection(int control_socket, int data_socket){
  if(write_s(control_socket, "QUIT\r\n", NULL, TYPE_NO_READ) != 0){
    printf("Error closing the connection, exiting...\n");
    close(data_socket);
    close(control_socket);
    exit(1);
  }

  close(data_socket);
  close(control_socket);

  return 0;
}
