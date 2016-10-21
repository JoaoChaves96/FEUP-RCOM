#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>

#define BAUDRATE B9600
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

#define SEND 0
#define RECEIVE 1

#define FLAG 0x7E
#define A_SENDER 0x03
#define A_RECEIVER 0x01
#define UA_CODE 0x07
#define SET_CODE 0x03

char * SET = {FLAG, A_SENDER, SET_CODE, A_SENDER^UA, FLAG);
char * UA = {FLAG, A_SENDER, UA_CODE, A_SENDER^UA, FLAG};

int llopen(int gate, int type)
{
	//Create file descriptor
	int fileDescriptor = open(MODEMDEVICE, O_RDWR | O_NOCCTY);
	char byteReceived; //Control byte

	if(fileDescriptor < 0)
	{
		return 1;
	}

 	if ( tcgetattr(fd,&oldtio) == -1) { /* save current port settings */
    	  perror("tcgetattr");
    	  exit(-1);
    	}

   	bzero(&newtio, sizeof(newtio));
   	newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
   	newtio.c_iflag = IGNPAR;
   	newtio.c_oflag = 0;

   	/* set input mode (non-canonical, no echo,...) */
   	newtio.c_lflag = 0;

   	newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
   	newtio.c_cc[VMIN]     = 1;   /* blocking read until 5 chars received */

	tcflush(fd, TCIOFLUSH); //Cleans the buffer

    if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }

    printf("New termios structure set\n");



	if(type == SEND) //Sends the SET message and waits for an answer
	{
		write(fd, &SET, 1);
		//TODO
	}
	else if(type == RECEIVE)
	{
		read(fd, &byteReceived, 1);
		write(fd, &UA, 1); //Sends the UA back to the sender
	}
	else //In case of error
	{
		return 1; 
	} 
}
