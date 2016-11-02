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
static unsigned char R;
static unsigned char x;

int receiveTrama(int fd, char *trama);

int waitForAnAnswer(Type command);

void updateState(State *state, Type type, unsigned char rByte);

int validTrama(char * answer);

void stuffTrama(char * trama, int * index);

int isStuffed(char * trama, int index, int tramaLength);

int deconstructTrama(char * dest, char * src, int length, unsigned char R);

int checkBytes(char * buf, int end,unsigned char S);

int destuffTrama(char * trama, int tramaLength);

int createTrama(char *trama, char *packet, int packetLength, unsigned char control);

void resend(char * buffer, unsigned int length);

void connect(char * buffer, unsigned int i);

struct termios oldtio,newtio;


int llopen(const char* path, int type)
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

		newtio.c_cc[VTIME] = 0;
		newtio.c_cc[VMIN] = 0;


	tcflush(fd, TCIOFLUSH); //Cleans the buffer

    if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }

    //printf("llopen: New termios structure set\n");

		configureAlarm(3);

	if(type == SEND) //Sends the SET message and waits for an answer
	{

		//printf("llopen: type SEND\n");
		write(fd, SET_PACKET, TRAMA_LENGTH);
		//printf("llopen: Set_Packet sent to receiver\n");

		setAlarm(resend, (char *) SET_PACKET, TRAMA_LENGTH);

		ctype = UA_S;

		if(waitForAnAnswer(ctype) != 0){
			printf("llopen: failed to connect to the receiver... Returning...\n");
			return -1;
		}

		stopAlarm();
		printf("llopen: UA_Packet received from receiver\n");

	}
	else if(type == RECEIVE)
	{
		setAlarm(connect,"", 0);
		//printf("llopen: type RECEIVE\n");
		ctype = SET;
		printf("llopen: waiting for answer\n");
	//	printf("llopen: waitforananswer value = %d\n", waitForAnAnswer(ctype));
		if(waitForAnAnswer(ctype) != 0){
			printf("llopen: failed to connect to the sender... Returning...\n");
			return -1;
		}
		stopAlarm();
		write(fd, UA_SENDER_PACKET, TRAMA_LENGTH); //Sends the UA back to the sender
		printf("llopen: write UA\n");
	}

	printf("llopen: Returning... \n");
	return fd;
}

/**
* Length = full length of packet (flags + data = 104)
*/
int llwrite(int fd, char * packet, int length){
	x = 0;
	char trama[255]; //tem de ser mudado
	unsigned char control = 0;
	control <<=6;
	int rByte,r, received = 1;
	char S[TRAMA_LENGTH];
	int j;
	//int timeout = 1;
	//printf("packet: ");
	for(j = 0; j < length; j++){
		//printf("%d %02X\t ",j, 0xFF & packet[j]);
	}
	printf("\n");
	int size = createTrama(trama,packet,length,control); //Length should be 100 + 4; Size = Full size of trama (TFlags + PHeader+ Data+ TFlags)
	//printf("llwrite: created trama with size %d\n", size);
	do{
		write(fd,trama,size);

		setAlarm(resend, trama, size);
		int it;
		for(it = 0; it < size; it++)
		{
			//printf("trama%d: %02x\n", it, trama[it]);
		}
		//sleep(7);
		/*int i = 0;
		for(i = 0; i < size; i++)
			printf("0x%02X\t", 0xFF & trama[i]);
		printf("\n");*/


		printf("llwrite: Trama sent. Waiting for answer...\n");
		/*if(received == 1) //To avoid setting the alarm again when an error occurs
			setAlarm(ALARM_SEC);*/
		rByte=0;

		while(rByte != TRAMA_LENGTH){
			r = read(fd,S+rByte,1);

			if(alarmActivated){
				printf("Connection timed out...\n");
				return -1;
			}
			/*if(n_timeouts == timeout) //to exit while cicle
			break;*/

			if(r){
				rByte++;
			}
		}

		stopAlarm();

		/*if (n_timeouts == timeout){
			timeout++;
			continue;
		}*/

		printf("llwrite: Answer received!\n");

		received = validTrama(S); //Verify if received an RR

		if(received == 0){
			//printf("llwrite: Invalid answer\n");
			tcflush(fd, TCIOFLUSH);
		}
		else if(received == -1)
		{
			//printf("llwrite: received REJ\n");
			tcflush(fd, TCIOFLUSH);
		}

	}while(received != 1);
		//printf("trama ");
		for(j = 4; j < length; j++){
		//printf("%d %02X\t ",j-4, 0xFF & trama[j]);
	}
	//printf("\n");
	//stopAlarm();
	//printf("llwrite: Valid answer!\n");
	x = (x+1) % 2;
	//printf("llwrite: Returning...\n");
	return size;
}

/**
* Returns size of data blocks int the trama
*/
int llread(int fd, char* packet){

R = 1;
unsigned char control;
int stuffedSize, tramaSize, valid, r;
char trama[512];
unsigned char S[5];

//printf("llread: Initializing...\n");
do{
	stuffedSize = receiveTrama(fd, trama);
	if(stuffedSize == -1)
	{
		printf("llread: failed to receive Trama\n");//TODO create counter que chega a 3 e manda abaixo
		return -1;
	}
	printf("llread: received Trama\n");
	tramaSize = destuffTrama(trama, stuffedSize);
	//printf("llread: tramaSize=%d",tramaSize);
	//printf("llread: destuffed Trama\n");
	r = deconstructTrama(packet, trama, tramaSize, R);
	//printf("llread: size of trama info: %d\n", r);
	//printf("llread: deconstructed Trama\n");
	if(r >1){ //Valid packet
		valid = TRUE;
		control = (R << 7) | RR_0; //== RR_1
		R = (R + 1) % 2;
	}

	else if(r == 1){ //repeated trama
		valid = FALSE;
		control = (R << 7) | RR_0;
	}

	else{ //Invalid trama
		valid = FALSE;
		control = (R << 7) | REJ_0;
	}

	S[0] = FLAG;
	S[1] = A_SENDER;
	printf("control byte sent: %02X\n", control);
	S[2] = control;
	S[3] = S[1] ^ S[2];
	S[4] = FLAG;

	write(fd, S, 5);
	//printf("llread: Answering the sender\n");
} while(!valid);

	//printf("llread: Returning...\n");
	return r; // r is never 1, because when r is one, the cycle repeats
}

int llclose(int fd, int programType){

	Type type;

	if(programType == SEND){
		write(fd, DISC_SENDER_PACKET, TRAMA_LENGTH);

		setAlarm(resend, (char *) DISC_SENDER_PACKET, TRAMA_LENGTH);
		type = DISC_R;

		if(waitForAnAnswer(type) != 0){
			printf("Failed to connect to the receiver...\n");
			return -1;
		}
		stopAlarm();
		write(fd, UA_RECEIVER_PACKET, TRAMA_LENGTH);
		sleep(2);
	}
	else if(programType == RECEIVE){
		setAlarm(connect, "", 0);
		type = DISC_S;

		if(waitForAnAnswer(type) != 0){
			printf("Failed to connect to the receiver...\n");
			return -1;
		}
		stopAlarm();
		write(fd, DISC_RECEIVER_PACKET, TRAMA_LENGTH);

		setAlarm(connect, "", 0);
		type = UA_R;

		if(waitForAnAnswer(type) != 0){
			printf("Failed to connect to the receiver...\n");
			return -1;
		}
	}
	stopAlarm();
	if (tcsetattr(fd, TCSANOW, &oldtio) == -1){
			perror("tcsetattr");
			return -1;
	}

	close(fd);
	return 0;
}

/**
* trama: trama that will bem created
* buf: packet created before
* control: 0 or 1, just for verification purposes
* packetLength: length of full packet = 4flags + 100data
*/
int createTrama(char *trama, char *packet, int packetLength, unsigned char control){
	//printf("createTrama: Initializing...\n");
	int i=0;
	int count;
	trama[i] = FLAG;
	i++;
	trama[i]=A_SENDER;
	i++;
	trama[i]=control;
	//printf("createTrama: counter module 2 = %02x\n", control);
	i++;

	unsigned char BCC1 = A_SENDER ^ control;
	//printf("createTrama: BCC1: %02X\n", BCC1);
	unsigned char BCC2 = 0x00;
	//printf("createTrama: packetLength = %d\n", packetLength);


	trama[i]=BCC1;
	//printf("createTrama: BCC1 atributed\n");
	i++;
	if(BCC1 == FLAG || BCC1 == ESCAPE) //If it needs stuffing
	{
		//printf("createTrama: BCC1 needs stuffing\n");
		stuffTrama(trama, &i); //Reference is passed, because if it is stuffed, then the index must ++
		//printf("createTrama: BCC1 stuffed\n");
	}

	for(count =0; count < packetLength; count++, i++){//Write packet in the trama TODO Tinha packetLength-1 ALTERACAO
		BCC2 = BCC2 ^ packet[count];
		trama[i] = packet[count];
		//printf("createTrama: trama[%d] atributed\n", i);
		if(trama[i] == FLAG || trama[i] == ESCAPE) //If it needs stuffing
		{
			//printf("createTrama: trama[%d] needs stuffing\n", i);
			stuffTrama(trama, &i);
			//printf("createTrama: trama[%d] stuffed\n", i);
		}
	}

	trama[i]=BCC2;
	//printf("createTrama: BCC2 atributed\n");
	if(BCC2 == FLAG || BCC2 == ESCAPE) //If it needs stuffing
	{
		//printf("createTrama: BCC2 needs stuffing\n");
		stuffTrama(trama, &i);
		//printf("createTrama: BCC2 stuffed\n");
	}
	i++;
	trama[i]=FLAG;
	//printf("createTrama: ENDFLAG atributed\n");
 	int j;
	for(j=0; j <=i;j++ ){
		//printf("createTrama: trama %d: %02x\n", j, (unsigned char)trama[j]);
	}

	//printf("createTrama: Returning...\n");
	return i+1; //Indexes start in 0, so size = lastindex + 1

}

int receiveTrama(int fd, char *trama){

	int i=0;
	int r=0;
	State state;
	unsigned char rByte;
	/*int timeout = 1;
	int failedShit = 0;*/

	state = START_RC;

	setAlarm(connect, "", 0);
	//setAlarm(ALARM_SEC);
	//printf("receiveTrama: Alarm Set\n");
	while(state != STOP_RC){

		r = read(fd, &rByte, 1);
		if(r!=1){
			if(alarmActivated){
				printf("receiveTrama: Alarm Activated \n");
				return -1;
			}
			else
			continue;
		}
		//printf("receiveTrama: read byte\n");

		/*if(alarmActivated){
			printf("receiveTrama: Alarm Activated \n");
			return -1;
		}*/

		if(r)
		{
		/*	if(failedShit)
			{
				printf("rbyte: %02x\n", rByte);
s				failedShit = 0;
			}*/
			//printf("receiveTrama: found byte %02x\n", rByte);
			switch(state){
				case START_RC:
					if(rByte == FLAG){
						trama[i] = rByte;
						i++;
						state = F_RC;
						//printf("receiveTrama: state=F_RC\n");
					}
					break;

				case F_RC:
					if(rByte == A_SENDER){
						trama[i] = rByte;
						i++;
						state = A_RC;
						//
					}
					else if(rByte == FLAG);
					else{
						state=START_RC;
						//printf("receiveTrama: state=START_RC\n");
					}
					break;

				case A_RC:
					if(rByte != FLAG){ // rByte == C ?
						trama[i] = rByte;
						i++;
						state = C_RC;
						//printf("receiveTrama: state=C_RC\n");
					}
					else{
						state = F_RC;
						//printf("receiveTrama: state=F_RC\n");
					}
					break;

				case C_RC:
					if(rByte != FLAG){ // rByte == A_SENDER ^C?
						trama[i] = rByte;
						i++;
						state = BCC;
						//printf("receiveTrama: state=BCC\n");
					}
					else{
						state = F_RC;
						//printf("receiveTrama: state=F_RC\n");
					}
					break;

				case BCC:
					if(rByte == FLAG){
						//printf("receiveTrama: LastByte\n");
						trama[i] = rByte;
						i++;
						state = STOP_RC;
						//printf("receiveTrama: state=STOP_RC\n");
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
/*			setAlarm(ALARM_SEC);
		}
		if(n_timeouts == timeout){
			printf("\n\n\nfailed to read!!! \n\n\n");
			i = 0;
			timeout++;
			failedShit = 1;
			state = START_RC;
		}*/
	}

	stopAlarm();
	return i;

}

int waitForAnAnswer(Type command){

	unsigned char rByte;
	State state;
	int r;

	state = START_RC;

	//sleep(2);
	//setAlarm(ALARM_SEC);
	printf("waitForAnAnswer: Alarm set\n");
	while(state != STOP_RC){  //while it doesnt receive the STOP byte
		printf("waitforananswer: before read\n");
		r = read(fd, &rByte, 1);
		printf("waitforananswer: alarmactivated: %d\n", alarmActivated);
		if(alarmActivated){
			printf("waitForAnAnswer: Alarm Activated \n");
			return -1;
			//setAlarm(3);
			//printf("Connection timed out...");
			//return -1;
		}

		if(r)
		{
			updateState(&state, command, rByte);
			printf("waitForAnAnswer: Updated state \n");
			//setAlarm(ALARM_SEC);
			printf("waitForAnAnswer: Alarm set\n");
		}
	}

	printf("waitForAnAnswer: Received Answer \n");

	//stopAlarm();

	printf("waitForAnAnswer: Stopped Alarm \n");
	return 0;
}

void updateState(State *state, Type type, unsigned char rByte){ //TheUpdate
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

/**
* Return: 1 if valid
*/
int validTrama(char *answer){
	//printf("validTrama: Validating answer...\n");
	//printf("%02X\n", S[2]);
	unsigned char control;
	//printf("validTrama: supposed{%02x, %02x, %02x, %02x, %02x}\n", FLAG, A_SENDER, RR_1, A_SENDER^RR_1, FLAG);
	control = (answer[2] << 1); //TODO & 0x7F; porque?
	control = (control >> 1);
	printf("control trama: %02X\n", control);
	//printf("%02X\n", control);
	//printf("validTrama: answer{%02x, %02x, %02x, %02x, %02x}\n", answer[0], answer[1], answer[2], answer[3], answer[4]);

/*	int RR_test = (answer[0] == FLAG) &&
	(answer[1] == A_SENDER) &&
	(control == RR_1) &&
	(answer[3] == (answer[1] ^ answer[2])) &&
	(answer[4] == FLAG);

	int REJ_test = (answer[0] == FLAG) &&
	(answer[1] == A_SENDER) &&
	(control == REJ_1) &&
	(answer[3] == (answer[1] ^ answer[2])) &&
	(answer[4] == FLAG);

	if(RR_test == 1)
	{
		//printf("validTrama: answer=RR\n");
		return 1;
	}
	else if(REJ_test == 1)
	{
		//printf("validTrama: answer=REJ\n");
		return -1;
	}
	else
	{
		//printf("validTrama: invalid answer\n");
		return 0;
	}*/
	return ((answer[0] == FLAG) &&
		(answer[1] == A_SENDER) &&
		(control == RR_0) &&
		(answer[3] == (answer[1] ^ answer[2])) &&
		(answer[4] == FLAG));

}

void stuffTrama(char * trama, int * index)
{
	unsigned char temp = trama[*index];
	trama[*index] = ESCAPE;
	trama[(*index)+1] = temp ^ XOR_OCTET;
	(*index)++; //To avoid conflicts outside the function
	return;
}

int destuffTrama(char * trama, int tramaLength)
{
	int i, j=3;
	int bytesAfterDestuffing = tramaLength;
	unsigned char * temp = malloc(tramaLength);
	//memcpy(temp, trama, tramaLength);
	temp[0] = trama[0];
	temp[1] = trama[1];
	temp[2] = trama[2];
	for(i = 3; i < tramaLength; i++, j++) //Because the 3 first ones are constant and not stuffed
	{/*
		int verifyStuffing = isStuffed(trama, i, tramaLength);
		if(verifyStuffing == FLAG_STUFFED)
		{
			//DESTUFF

			int tramaSizeUntilEnd = tramaLength-i;
			if(i == 84)
			{
				printf("destuffTrama: trama[84]=%02x, %02x\n", trama[i], trama[i+1]);
			}
			memcpy(temp+i, &trama[i+1], tramaSizeUntilEnd);
			memcpy(trama, temp, tramaLength);
			trama[i] = FLAG;
			if(i == 84)
			{
				printf("destuffTrama: trama[84]=%02x, %02x\n", trama[i], trama[i+1]);
			}
			bytesAfterDestuffing--;

		}
		else if(verifyStuffing == ESCAPE_STUFFED)
		{
			//DESTUFF

			int tramaSizeUntilEnd = tramaLength-i;
			memcpy(temp+i, &trama[i+1], tramaSizeUntilEnd);
			memcpy(trama, temp, tramaLength);
			trama[i] = ESCAPE;
			bytesAfterDestuffing--;
		}*/
			int verifyStuffing = isStuffed(trama, i, tramaLength);
			if(verifyStuffing == FLAG_STUFFED)
			{
				temp[j] = FLAG;
				i++;
				bytesAfterDestuffing--;
			}
			else if(verifyStuffing == ESCAPE_STUFFED)
			{
				temp[j] = ESCAPE;
				i++;
				bytesAfterDestuffing--;
			}
			else
			{
				temp[j] = trama[i];
			}
	}
//	printf("destuffTrama: Done. Length after destuffing=%d\n", bytesAfterDestuffing);
	for(i = 0; i < bytesAfterDestuffing; i++)
	{
	//	printf("destuffTrama: trama[%d]=%02x\n" , i, temp[i]);
	}
	memcpy(trama, temp, bytesAfterDestuffing);
	free(temp);
	return bytesAfterDestuffing;
}

int isStuffed(char * trama, int index, int tramaLength)
{
	//printf("isStuffed: index=%d, tramaLength=%d\n", index, tramaLength);
	if(index != tramaLength)
	{
		if(trama[index] == ESCAPE && trama[index+1] == 0x5e)
			return FLAG_STUFFED;
		else if(trama[index] == ESCAPE && trama[index+1] == 0x5d)
			return ESCAPE_STUFFED;
		else
			return NOT_STUFFED;
	}
	else
		return NOT_STUFFED;
}

/**
* Return: SIZE OF packet (> 1) - ok, 1 - repeated, -1 - invalid
*/
int deconstructTrama(char * dest, char * src, int length, unsigned char R){
	int D1_INDEX = 4; //(BCC index = 3) + 1
	int END_INDEX = length-1;
	int BCC2_INDEX = length -2;
	unsigned char validBCC2 = 0x00;
	//printf("deconstructTrama: End_packet:%d", END_INDEX);
	unsigned char S = (R + 1) % 2;
	//printf("deconstructTrama: S:%02x\n", S);
	if((src[2] >> 6) == R){ // C index = 2, verifies counter. They must be different
		//printf("deconstructTrama: Repeated packet...\n");
		return 1;
	}

	else if(!checkBytes(src, END_INDEX, S))
	{
		//printf("deconstructTrama: Invalid Trama\n");
		return -1;
	}

	int i;

	for(i = 0; i+D1_INDEX < BCC2_INDEX; i++){
		dest[i] = src[i+D1_INDEX];
		validBCC2 = validBCC2 ^ src[i+D1_INDEX];
	}

	//printf("validBCC2: %02x; src[BCC2]: %02x\n", validBCC2, src[BCC2_INDEX]);
	if(validBCC2 != (unsigned char) src[BCC2_INDEX]) //checks parity of BCC2
	{
		//printf("deconstructTrama: parity check BCC2 failed\n");
		return -1;
	}
//	printf("deconstructTrama: BCC2_INDEX = %d,   D1_INDEX = %d\n", BCC2_INDEX, D1_INDEX);
	return i; //Length of the packet
}

int checkBytes(char * src, int end, unsigned char S){
	//printf("END %d\n", end);
	//printf("checkBytes: src=%02X %02X %02X %02X %02X \n", src[0], src[1], src[2], src[3], src[end]);
	//printf("checkBytes: supposed=%02X %02X %02X %02X %02X \n", FLAG, A_SENDER, S << 6, A_SENDER^(S<<6), FLAG);
	return (src[0] == FLAG && src[1] == A_SENDER && src[2] == (S << 6)
	&& src[3] == (src[1]^src[2]) && src[end] == FLAG);
}

void resend(char * buffer, unsigned int length){
	write(fd, buffer, length);
}

void connect(char * buffer, unsigned int i){
	return;
}
