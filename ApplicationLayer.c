#include "ApplicationLayer.h"
#include "DataLink.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

int startApp(Application * app, const char * path, int type, int status, const char * fileName, unsigned int nameLength){
	int r;
	unsigned int nameL;
	const char* fileN;

	app->status = type;
	if(type == SEND){
		r = openW(app, path, fileName, nameLength);
	}
	else if (type == RECEIVE){
		r = openR(app, path);
	}

	return r;


}

int openW(struct Application * app, const char * path, const char * filename, unsigned int nameLength){
	FILE * file;

	app->nameLength = nameLength;
	app->fileName = (char*) malloc(sizeof(app->nameLength));

	if(app->fileName == NULL)
		return -1;

	strcpy(app->fileName, filename);

	file = fopen(app->fileName, "rb"); // non text file
	if(file == NULL)
		return -1;

	fseek(file, 0, SEEK_END); //get file size
	app->fileSize = ftell(file); // returns current file position
	fseek(file, 0, SEEK_SET); // return to the beginning of the file

	app->buf = (char*) malloc(app->fileSize);
	if(app->buf == NULL)
		return -1;

	fread(app->buf, app->fileSize, sizeof(char), file);
	fclose(file);

	app->filedes = llopen(path, SEND);
	if(app->filedes == -1)
		return -1;

	return 0;
}

int openR(struct Application * app, const char *path){
	app->filedes == llopen(path, RECEIVE);

	if(app->filedes == -1)
		return -1;

	return 0;
}

int writeApp(struct Application app){
	int pSize = MIN_CONTROL_P_SIZE + app.nameLength;

	char *c_packet = malloc(pSize);

	startPacket(app, c_packet, START);

	if(llwrite(app.filedes, c_packet, pSize) == -1){
		printf("Unable to send start control packet, connection timed out....\n");
		return -1;
	}

	int packetsToSend = 1;
	/*
	----- NAO APAGAR!!!! -------
	int packetsToSend = app.fileSize / PACKET_SIZE
	int bytesLeft = app.fileSize % PACKET_SIZE;

	*/


	//Sends the Data Packets
	int i = 0;
	int size;
	for(i; i < packetsToSend; i++)
	{
		/*
		------ NAO APAGAR!! -----
		if(bytesLeft < PACKET_SIZE)
			size = bytesLeft;
		else
			size = PACKET_SIZE;
*/
		size = app.fileSize;
		char * d_packet = malloc(size+MIN_DATA_P_SIZE);

		dataPacket(app, d_packet, i);
		if(llwrite(app.filedes, d_packet, size) == -1)
		{
			printf("Unable to send data packet %d, connection timed out....\n", i);
			return -1;
		}

		free(d_packet);
	}


	startPacket(app, c_packet, END);
	if(llwrite(app.filedes, c_packet, pSize) == -1){
		printf("Unable to send end control packet, connection timed out....\n");
		return -1;
	}

	free(c_packet);

	return 0;
}

int readApp(struct Application app)
{
	FILE * file = fopen(app.filename, "w");

	char * startPacket = malloc(MAX_CONTROL_P_SIZE);
	char * endPacket = malloc(MAX_CONTROL_P_SIZE);

	llread(app.filedes, startPacket); //Reads the start packet
	if(verifyControlPacket(startPacket, START) != 1)
	{
		printf("readApp: StartPacket verification failed \n");
		return -1;
	}

	int fileSize;
	int nameSize;
	memcpy(&fileSize, startPacket[3], startPacket[2]); //Reads the file size from the packet
	memcpy(&nameSize, startPacket[8], 1); //Reads the file's name size from the packet
	char * fileName = malloc(nameSize);
	memcpy(fileName, startPacket[9], startPacket[8]); //Reads the file's name from the packet

	int numPackets = 1;//NAO APAGAR!!!!!! ceil((float) (fileSize/PACKET_SIZE);
	int i;
	char * dataPacket = malloc(4+MAX_DATA_P_SIZE);
	for(i = 0; i < numPackets; i++) //Reads each packet, and copies the data to the new file
	{
		int packetSize = llread(app.filedes, dataPacket); //Assumindo que llread retorna o numero de bytes com dados (sem contar com o Packet header)
		fwrite(dataPacket[4], sizeof(char), packetSize-MIN_DATA_P_SIZE, file); //Writes the data to the new file
	}

	llread(app.filedes, endPacket); //Reads the end packet
	if(verifyControlPacket(endPacket, END) != 1)
	{
		printf("readApp: EndPacket verification failed \n");
		return -1;
	}

	return 0;

}

void startPacket(struct Application app, char * c_packet, char CONTROL_FLAG){

	c_packet[0] = CONTROL_FLAG;
	c_packet[1] = 0;
	c_packet[2] = sizeof(int);
	memcpy(c_packet[3], app.fileSize, sizeof(int));
	c_packet[7] = 1;
	c_packet[8] = app.nameLength;
	memcpy(c_packet[9], app.fileName, app.nameLength);

	return;
	
}

void dataPacket(struct Application app, char * d_packet, int serialNumber){
	d_packet[0] = DATA_CONTROL;
	d_packet[1] = serialNumber % 255;
	d_packet[2] = app.fileSize % 256;
	d_packet[3] = (int)(app.fileSize / 256);
	memcpy(d_packet[4], app.buf, app.fileSize); //TODO TEMPORARIO. TEM DE SER TROCADO POR PACKETS MAIS RECENTES

	return;

}

/**
* Returns:
* 1 = Sucess | 0 = Failure | -1 = Error
*/
int verifyControlPacket(char * packet, int type)
{
	if(packet == NULL)
	{
		printf("verifyControlPacket: EMPTY PACKET \n");
		return -1; //Error
	}
	else if(type == packet[0])
	{
		return 1; //Its the correct control packet
	}
	else
	{
		return 0; //Its an incorrect control packet
	}
}