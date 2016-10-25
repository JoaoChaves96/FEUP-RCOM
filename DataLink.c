#include "DataLink.h"

char SET[] = {FLAG, A_SENDER, SET_CODE, A_SENDER ^ SET_CODE, FLAG};
char UA[] = {FLAG, A_SENDER, UA_CODE, A_SENDER ^ UA_CODE, FLAG};
char RR0[] = {FLAG, A_SENDER, RR_0, A_SENDER^RR_0, FLAG}; //TODO verificar se o BCC dá numero par de 1
char RR1[] = {FLAG, A_SENDER, RR_1, A_SENDER^RR_1, FLAG};
char REJ0[] = {FLAG, A_SENDER, REJ_0, A_SENDER^REJ_0, FLAG};
char REJ1[] = {FLAG, A_SENDER, REJ_1, A_SENDER^REJ_1, FLAG};


struct termios oldtio,newtio;


int llopen(char* path, int type)
{
	//Create file descriptor
	int fd = open(path, O_RDWR | O_NOCTTY);
	char byteReceived[5]; //Control byte

	configureAlarm();

	if(fd < 0)
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
		char * answer;
		write(fd, &SET, 5);
		answer = waitForAnswer(fd, ALARM_SEC, UA_CODE, UA_SIZE);
		if(answer != NULL) //Sucess
		{
			printf("enviou com sucesso\n");
			free(answer);
			return fd;
		}
		else //Fail
		{	
			printf("falhou com sucesso\n");
			free(answer);
			return NULL;
		}
	}
	else if(type == RECEIVE)
	{
		read(fd, &byteReceived, 5);
		printf("BYTERECEIVED: %d %d %d %d %d \n", byteReceived[0], byteReceived[1], byteReceived[2], byteReceived[3], byteReceived[4]);
		write(fd, &UA, 5); //Sends the UA back to the sender
	}
	else //In case of error
	{
		return 1;
	}
}

// Used by the SENDER, waits for the RECEIVER to respond
char * waitForAnswer(int fd, int sec, char * command, int commandSize)
{
	char * msg = malloc(sizeof(char)*commandSize);
	int receivedCommand = 0;

	int retries = 0;
	for(retries; retries < MAX_RETRIES && retries >= 0; retries++)
	{
		// Turns on the alarm for timeout
		setAlarm(sec);
		printf("Retries : %d \n", retries);
		while(alarmActivated == 0) //While not timeout
		{
			read(fd, msg, commandSize); //Read incoming message
			/*
			if(!strcmp(msg, command)) //If a command is received
				receivedCommand = 1;
				retries = -1; //In order not to repeat the for cycle
				stopAlarm();
				break;*/
			printf("MSG: %d %d %d %d %d \n", msg[0], msg[1], msg[2], msg[3], msg[4]);
			if(verifyTrama(msg, commandSize))
			{
				printf("Trama accepted\n");
				receivedCommand = 1;
				retries = -2; //In order not to repeat the for cycle
				stopAlarm();
				break;
			}		
		}
	}

	if(retries == -1) //Found a command
	{
		printf("Found a command \n");
		return msg;
	}
	else if(retries == MAX_RETRIES) //Number of retries reached the limit
	{
		printf("Reached Max Retries \n");
		free(msg);
		return NULL;
	}
	else //Error
	{
		printf("Error \n");
		free(msg);
		return NULL; //TODO Arranjar uma maneira de passar um erro
	}
}



/**
* Type can be 0x03 if sent by SENDER, or received by RECEIVER == type 0
* Or 0x01 if sent by RECEIVER, or received by SENDER == type 1
*/
char * createITrama(char * package, int package_length, int type)
{
	char * trama = malloc(sizeof(char)*(package_length + 6));

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
	trama[2] = 0x40; //PROVISORIO
	trama[3] = trama[1]^trama[2];
	trama[4] = *package;
	trama[4+package_length] = 0x00 ^ *package;
	trama[4+package_length+1] = FLAG;

	return trama;
}

int verifyTrama(char * trama, int tramaSize)
{
	if(trama[0] != FLAG)
	{
		printf("FLAG WRONG: %d\n", trama[0]);
		return FALSE;
	}
	if(trama[1] != A_SENDER)
	{
		printf("A_SENDER WRONG: %d\n", trama[1]);
		return FALSE;
	}

	//TODO trama[2]

	int ones = countOnes(trama[3]);
	printf("number of ones of BCC: %d \n", ones);
	printf("TRAMA: %02X %02X %02X %02X %02X \n", trama[0], trama[1], trama[2], trama[3], trama[4]);
	if((trama[1]^trama[2]) != trama[3]) //Meaning a odd number of ones
	{
		printf("BCC WRONG: %02X %02X\n",trama[1]^trama[2], trama[3]);
		return FALSE;
	}
	if(trama[tramaSize-1] != FLAG)
	{
		printf("END FLAG WRONG: %d\n", trama[tramaSize-1]);
		return FALSE;
	}
	printf("GREAT SUCCESS: TRAMA OK \n");
	return TRUE;
}

