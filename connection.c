/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <signal.h>

#define FLAG 0x7E
#define A 0x03
#define C_SET 0x03
#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1
#define UA 0x07

int flag=1;
int fd;
int to = 0;

unsigned char set[5];

struct applicationLayer {
	int fileDescriptor; /*Descritor correspondente à porta série*/
	int status; /*TRANSMITTER | RECEIVER*/
};

void readBuffer(){

	unsigned char buf_test[5];
	unsigned state = 0;

	//alarm(3);
	printf("Dentro readBuffer()\n");

while(state != 5){
	if(!flag)
		break;
	switch(state){
	
		//Start state
		case 0:	read(fd, buf_test, 1);
				printf("case 0");
				if(buf_test[0] == FLAG)
					state = 1;
				else state = 0;
		
		//FLAG RCV state
		case 1: read(fd, buf_test + 1, 1);
				printf("case 1");
				if(buf_test[1] == A)
					state = 2;
				else if(buf_test[1] == FLAG)
					state = 1;
				else state = 0;	
		
		//A RCV state
		case 2: read(fd, buf_test + 2, 1);
				printf("case 2");
				if(buf_test[2] == UA)
					state = 3;
				else if(buf_test[2] == FLAG)
					state = 1;
				else state = 0;
		
		//UA RCV state		
		case 3: read(fd, buf_test + 3, 1);
				printf("case 3");
				if(buf_test[3] == A ^ UA)
					state = 4;
				else if(buf_test[3] == FLAG)
					state = 1;
				else state = 0;

		//BCC OK state	
		case 4: read(fd, buf_test + 4, 1);
				printf("case 4");				
				if(buf_test[4] == FLAG)
					state = 5;
				else state = 0;
			
	}
}
flag=0;
printf("\nSET[]: %d %d %d %d %d\n", buf_test[0], buf_test[1], buf_test[2], buf_test[3], buf_test[4]);
printf("Qualquer fim readBuffer()\n");
}

void receive(){

	printf("Dentro atende()\n");	
	to++;

	if(to>3){
		flag=0;
		exit(1);
		//return;
	}
	else write(fd, set, 5);

	alarm(3);
	//readBuffer();
	return;
}

int main(int argc, char** argv){

	flag=0;
	unsigned char buf[255];
	int res;
	int STOP = FALSE;
	(void) signal(SIGALRM, receive);
    struct termios oldtio,newtio;
    
	fd = open(argv[1], O_RDWR | O_NOCTTY );
    if (fd <0) {perror(argv[1]); exit(-1); }

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



  /* 
    VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a 
    leitura do(s) próximo(s) caracter(es)
  */



    tcflush(fd, TCIOFLUSH);

    if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }

    printf("New termios structure set\n");

	set[0] = FLAG;
	set[1] = A;
	set[2] = C_SET;
	set[3] = set[1]^set[2];
	set[4] = FLAG;

	printf("SET[]: %d %d %d %d %d\n", set[0], set[1], set[2], set[3], set[4]);

	write(fd, set, 5);
	//alarm(3);

	while(STOP == FALSE){
		printf("depois alarm(3)\n");
		res = read(fd, buf, 255);
		
		if(buf[4] == FLAG){
			STOP = TRUE;
			printf("buf[]: %d %d %d %d %d\n", buf[0], buf[1], buf[2], buf[3], buf[4]);
		}

	}

	//readBuffer();

	printf("Depois readBuffer()\n");

	//printf("buf_test %s\n", buf_test);
	printf("Apos maquina estados read()\n");

	sleep(3); 


    if ( tcsetattr(fd,TCSANOW,&oldtio) == -1) {
      perror("tcsetattr");
      exit(-1);	
	}

	close(fd);
return 0;
}
