#include "DataLink.h"

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
		if(waitForAnswer(fileDescriptor, ALARM_SEC, UA, UA_SIZE) != NULL) //Sucess
		{
			return fileDescriptor;
		}
		else //Fail
		{
			return NULL;
		}
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


int llread(int fd, char * buffer)
{

}

int llwrite(int fd, char *buffer, int length)
{
	char * trama = (buffer, length, 0);

	//Send trama
	write(fd, buffer, (size_t)length);

	//Wait for RR/REJ
	waitForAnswer(fd, ALARM_SEC, )
	//If RR
		//Verify if N(r) is correct
	//If REJ
		//Resend trama
}

// Used by the SENDER, waits for the RECEIVER to respond
char * waitForAnswer(int fd, int sec, char * command, int commandSize)
{
	char * msg = malloc(sizeof(char));
	int receivedCommand = 0;

// Turns on the alarm for timeout
	setAlarm(sec);

	int retries = 0;
	for(retries; retries < MAX_RETRIES && retries >= 0; retries++)
		while(alarmActivated != 0) //While not timeout
		{
			read(fd, msg, commandSize); //Read incoming message
/*
			if(!strcpm(msg, command)) //If a command is received
				receivedCommand = 1;
				retries = -1; //In order not to repeat the for cycle
				stopAlarm();
				break;*/
			if(verifyTrama(msg, commandSize))
				receivedCommand = 1;
				retries = -1; //In order not to repeat the for cycle
				stopAlarm();
				break;
		}
	}

	if(retries == -1) //Found a command
	{
		return msg;
	}
	else if(retries == MAX_RETRIES) //Number of retries reached the limit
	{
		return NULL;
	}
	else //Error
	{
		return NULL; //TODO Arranjar uma maneira de passar um erro
	}
}



/**
* Type can be 0x03 if sent by SENDER, or received by RECEIVER == type 0
* Or 0x01 if sent by RECEIVER, or received by SENDER == type 1
*/
char * createITrama(char * package, int package_length, int type)
{
	char * trama = malloc(sizeof(char)*(package_length + 6);

	trama[0] = FLAG;
	if(type == 0)
		trama[1] = 0x03;
	else if(type == 1)
		trama[1] = 0x01;
	else
		return NULL;

	/*
	TODO
	trama[2] ??
	Aquele N(s) e N(r) estão sempre a alternar??
	*/
	trama[2] = 0x40 //PROVISORIO
	trama[3] = trama[1]^trama[2];
	trama[4] = *package;
	trama[4+package_length] = 0x00 ^ *package;
	trama[4+package_length+1] = FLAG;

	return trama;
}

int verifyTrama(char * trama, int tramaSize)
{
	if(trama[0] != FLAG)
		return FALSE;
	if(trama[1] != A_SENDER)
		return FALSE;

	//TODO trama[2]

	int ones = countOnes(trama[3]);
	if(ones % 2 == 1) //Meaning a odd number of ones
		return FALSE;
	if(trama[tramaSize-1] != FLAG)
		return FALSE;

	return TRUE;
}
