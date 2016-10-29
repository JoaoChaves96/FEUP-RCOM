#include "DataLink.h"

char SET_PACKET[] = {FLAG, A_SENDER, SET_CODE, A_SENDER ^ SET_CODE, FLAG};
char UA_SENDER_PACKET[] = {FLAG, A_SENDER, UA_CODE, A_SENDER ^ UA_CODE, FLAG};
char UA_RECEIVER_PACKET[] = {FLAG, A_RECEIVER, UA_CODE, A_RECEIVER ^ UA_CODE, FLAG};
char RR0_PACKET[] = {FLAG, A_SENDER, RR_0, A_SENDER^RR_0, FLAG}; //TODO verificar se o BCC dá numero par de 1
char RR1_PACKET[] = {FLAG, A_SENDER, RR_1, A_SENDER^RR_1, FLAG};
char REJ0_PACKET[] = {FLAG, A_SENDER, REJ_0, A_SENDER^REJ_0, FLAG};
char REJ1_PACKET[] = {FLAG, A_SENDER, REJ_1, A_SENDER^REJ_1, FLAG};
char DISC_SENDER_PACKET[] = {FLAG, A_SENDER, DISC_CODE, A_SENDER^DISC_CODE, FLAG};
char DISC_RECEIVER_PACKET[] = {FLAG, A_RECEIVER, DISC_CODE, A_RECEIVER^DISC_CODE, FLAG};

typedef enum{START_RC, F_RC, A_RC, C_RC, BCC,STOP_RC} State;
typedef enum{SET, UA_S, UA_R, DISC_S, DISC_R} Type;

int fd;

int createTrama(char *trama, char *buf, int length, char control);

int receiveTrama(int fd, char *trama);

int waitForAnAnswer(Type command);

void updateState(State *state, Type type, unsigned char rByte);

int validTrama(char *S);


struct termios oldtio,newtio;


int llopen(char* path, int type)
{

	Type ctype;
	//Create file descriptor
	fd = open(path, O_RDWR | O_NOCTTY);
	//char byteReceived[5]; //Control byte

	if(fd < 0)
	{	
		perror(path);
		return -1;
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


   	if (type == RECEIVE){
   	newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
   	newtio.c_cc[VMIN]     = 1;   /* blocking read until 5 chars received */
   	}

   	if (type == SEND){
   		newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
   		newtio.c_cc[VMIN]     = 0;   /* blocking read until 5 chars received */
   	}

	tcflush(fd, TCIOFLUSH); //Cleans the buffer

    if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }

    printf("New termios structure set\n");


    configureAlarm(3);

	if(type == SEND) //Sends the SET message and waits for an answer
	{
		write(fd, SET_PACKET, TRAMA_LENGTH);
		setAlarm(3, fd, SET_PACKET, TRAMA_LENGTH);

		ctype = UA_S;

		if(waitForAnAnswer(ctype) != 0){
			printf("failed to connect to the receiver...");
			return -1;
		}

		stopAlarm();

	}
	else if(type == RECEIVE)
	{
		ctype = SET;

		if(waitForAnAnswer(ctype) != 0){
			printf("failed to connect to the sender...");
			return -1;
		}

		write(fd, UA_SENDER_PACKET, TRAMA_LENGTH); //Sends the UA back to the sender
	}

	return fd;
}

int llwrite(int fd, char * buffer, int length){
	static unsigned char x = 0;
	char trama[255];
	unsigned char control = 0;
	control <<=6;
	int rByte,r, received;
	char S[TRAMA_LENGTH];


	int size = createTrama(trama,buffer,length,control);

	do{
		write(fd,trama,size);
		setAlarm(3, fd, trama, length);
		rByte=0;

		while(rByte != TRAMA_LENGTH){
			r = read(fd,S+rByte,1);
			if(!alarmActivated){
				printf("Connection timed out...\n");
				return -1;
			}
			if(r){
				rByte++;
			}
		}

		stopAlarm();

		received = validTrama(S);

		if(!received){
			tcflush(fd, TCIOFLUSH);
		}
	}while(!received);

	x = (x+1) % 2;

	return size;
}

/**
* Returns size of data blocks int the trama
*/
int llread(int fd, char* packet){
	//int pSize;

	/*do{
		pSIze = receiveTrama(fd, trama);
	}*/

	//read

	//destuff

	//(TODO: O RECEIVE TRAMA ESTA INCORRETO. NAO TEM EM CONTA O STUFFING)

	//Deconstruct header + tail

	return 0;
}

int llclose(int fd, int programType){

	Type type;

	if(programType == SEND){
		write(fd, DISC_SENDER_PACKET, TRAMA_LENGTH);

		type = DISC_R;

		if(waitForAnAnswer(type) != 0){
			printf("Failed to connect to the receiver...\n");
			return -1;
		}

		write(fd, UA_RECEIVER_PACKET, TRAMA_LENGTH);
		sleep(2);
	}
	else if(programType == RECEIVE){

		type = DISC_S;

		if(waitForAnAnswer(type) != 0){
			printf("Failed to connect to the receiver...\n");
			return -1;
		}

		write(fd, DISC_RECEIVER_PACKET, TRAMA_LENGTH);

		type = UA_R;

		if(waitForAnAnswer(type) != 0){
			printf("Failed to connect to the receiver...\n");
			return -1;
		}
	}

	if (tcsetattr(fd, TCSANOW, &oldtio) == -1){
			perror("tcsetattr");
			return -1;
	}

	close(fd);
	return 0;
}

int createTrama(char *trama, char *buf, int length, char control){

	int i=0;
	int count;

	trama[i] = FLAG;
	i++;
	trama[i]=A_SENDER;
	i++;
	trama[i]=control;

	unsigned char BCC1 = A_SENDER ^ control;
	unsigned char BCC2 = 0x00;

	i++;
	trama[i]=BCC1;
	if(BBC1 == FLAG || BCC1 == ESCAPE) //If it needs stuffing
	{
		stuffThatTrama(trama, i);
	}

	for(count =0; count < length; count++, i++){
		BCC2 = BCC2 ^ buf[count];
		trama[i] = buf[count];
		if(trama[i] == FLAG || trama[i] == ESCAPE) //If it needs stuffing
		{
			stuffThatTrama(trama, i);
		}
	}

	i++;
	trama[i]=BCC2;
	if(BCC2 == FLAG || BCC2 == ESCAPE) //If it needs stuffing
	{
		stuffThatTrama(trama, i);
	}
	i++;
	trama[i]=FLAG;

	return i;

}

int receiveTrama(int fd, char *trama){

	int i=0;
	State state;
	unsigned char rByte;

	state = START_RC;

	while(state != STOP_RC){
		read(fd, &rByte,1);

		switch(state){
			case START_RC:
				if(rByte == FLAG){
					trama[i] = rByte;
					i++;
					state = F_RC;
				}
				break;

			case F_RC:
				if(rByte == A_SENDER){
					trama[i] = rByte;
					i++;
					state = A_RC;
				}
				else if(rByte == FLAG);
				else{
					state=START_RC;
				}
				break;

			case A_RC:
				if(rByte != FLAG){
					trama[i] = rByte;
					i++;
					state = C_RC;
				}
				else{
					state = F_RC;
				}
				break;

			case C_RC:
				if(rByte != FLAG){
					trama[i] = rByte;
					i++;
					state = BCC;
				}
				else{
					state = F_RC;
				}
				break;

			case BCC:
				if(rByte == FLAG){
					trama[i] = rByte;
					i++;
					state = STOP_RC;
				}
				else{
					trama[i]=rByte;
					i++;
				}
				break;
			default:
				break;

		}
	}

	return i;

}

int waitForAnAnswer(Type command){

	unsigned char rByte;
	State state;
	int r;

	state = START_RC;

	sleep(2);
	while(state != STOP_RC){  //while it doesnt receive the STOP byte

		r = read(fd, &rByte, 1);

		if(alarmActivated){
			return -1;
			//setAlarm(3);
			//printf("Connection timed out...");
			//return -1;
		}

		if(r)
			updateState(&state, command, rByte);
	}

	stopAlarm();

	return 0;
}

void updateState(State *state, Type type, unsigned char rByte){
	switch(*state){
		case START_RC:
			if(rByte == FLAG){
				*state = F_RC;
			}
			break;

		case F_RC:

			if( ((type == SET) || (type == UA_S) || (type == DISC_S)) && rByte == A_SENDER)
				*state = A_RC;

			else if(((type == UA_R) || (type == DISC_R)) && rByte == A_RECEIVER)
				*state = A_RC;

			else if (rByte == FLAG); // didnt advance to next byte, stays in the same byte/state

			else
				*state = START_RC;

			/*if(((programType == SEND) && (rByte == A_RECEIVER)) || ((programType == RECEIVE) && (rByte == A_SENDER)))
				*(state++);
			else{
				printf("erro\n");  //TODO deu erro.
			}*/

			break;

		case A_RC:

			if (((type == UA_S) || (type == UA_R)) && rByte == UA_CODE)
				*state = C_RC;

			else if(type == SET && rByte == SET_CODE)
				*state = C_RC;

			else if(((type == DISC_S) || (type == DISC_R)) && rByte == DISC_CODE)
				*state = C_RC;

			else if(rByte == FLAG)
				*state = F_RC;
			else
				*state = START_RC;

			break;

			/*if(((programType == SEND) && (rByte == A_RECEIVER)) || ((programType == RECEIVE) && (rByte == A_SENDER)))
				*(state++);
			else{
				printf("erro\n");  //TODO deu erro.
			}*/

		case C_RC:
			if ((type == UA_S && rByte == (A_SENDER^UA_CODE)) || (type == UA_R && rByte == (A_RECEIVER^UA_CODE))
				|| (type == SET && rByte == (A_SENDER^SET_CODE)) || (type == DISC_S && rByte == (A_SENDER^DISC_CODE))
				|| (type == DISC_R && rByte == (A_RECEIVER^DISC_CODE)))
				*state = BCC;

			else if (rByte == FLAG)
				*state = F_RC;

			else
				*state = START_RC;

			break;

		case BCC:
			if(rByte == FLAG)
				*state = STOP_RC;
			else
				*state = START_RC;

			break;

		default:
			break;
	}
}

int validTrama(char *S){

	printf("%02X\n", S[2]);
	unsigned char control;
	control = S[2] & 0x7F;
	printf("%02X\n", control);


	return (
		(S[0] == FLAG) &&
		(S[1] == A_SENDER) &&
		(control == RR_0) &&
		(S[3] == (S[1] ^ S[2])) &&
		(S[4] == FLAG));

}

void stuffThatTrama(char * trama, int index)
{
	char temp = trama[index];
	trama[index] = ESCAPE;
	trama[index+1] = temp ^ XOR_OCTET;
	return;
}

void destuffThatTrama(char * trama, int tramaLength)
{
	int i;
	for(i = 3; i < tramaLength; i++)
	{
		int verifyStuffing = isStuffed(trama, i, tramaLength);
		if(verifyStuffing == FLAG_STUFFED)
		{
			//TODO DESTUFF
		}
		else if(verifyStuffing == ESCAPE_STUFFED)
		{
			//TODO DESTUFF
		}
	}
}

int isStuffed(char * trama, int index, int tramaLength)
{
	if(i != tramaLength)
	{
		if(trama[i] == ESCAPE && trama[i+1] == 0x5e)
			return FLAG_STUFFED;
		else if(trama[i] == ESCAPE && trama[i+1] == 0x5d)
			return ESCAPE_STUFFED;
		else
			return NOT_STUFFED;
	}
	else
		return NOT_STUFFED;
}
