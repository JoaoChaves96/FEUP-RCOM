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
	long int bufPointer;
	int nameLength;
	unsigned int fileSize;
};

/**
* Return: 0 = Sucess; -1 = error
*/
int startApp(struct Application * app, const char * path, int type, const char * fileName, unsigned int nameLength);

int openW(struct Application * app, const char * path, const char * filename, unsigned int nameLength);

int openR(struct Application * app, const char *path);

int writeApp(struct Application app);

int readApp(struct Application app);

void startPacket(struct Application app, char * c_packet, char CONTROL_FLAG);

void dataPacket(struct Application * app, char * d_packet, int serialNumber);

int verifyControlPacket(char * packet, int type);

#endif
