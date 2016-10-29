#ifndef APPLICATIONLAYER_H
#define APPLICATIONLAYER_H

#define DATA_CONTROL 1
#define START 2
#define END 3
#define MIN_CONTROL_P_SIZE 9
#define MAX_CONTROL_P_SIZE 256
#define MIN_DATA_P_SIZE 4 //DEPENDE DO TAMANHO MAXIMO DAS TRAMAS
#define MAX_DATA_P_SIZE 256

#define PACKET_SIZE 100 //Not used for now

struct Application{
	int status;
	int filedes;
	char* fileName;
	char * buf;
	int nameLength;
	unsigned int fileSize;
};

#endif