/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

#define FLAG 0x7e
#define A 0x03
#define SET_C 0x03
#define UA 0x07

volatile int STOP=FALSE;

int to = 0;
unsigned char set[5];
int fd;
int flag=1;

void readBuffer();

void atende(){

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
printf("Qualquer fim readBuffer()\n");

}

int main(int argc, char** argv)
{

	(void) signal(SIGALRM, atende);
    int c, res;
    struct termios oldtio,newtio;
    char buf[255];
    int i, sum = 0, speed = 0;
    
    if ( (argc < 2) || 
  	     ((strcmp("/dev/ttyS0", argv[1])!=0) && 
  	      (strcmp("/dev/ttyS1", argv[1])!=0) )) {
      printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
      exit(1);
    }

    fd = open(argv[1], O_RDWR | O_NOCTTY );
    if (fd <0) {perror(argv[1]); exit(-1); }

    if ( tcgetattr(fd,&oldtio) == -1) { /* save current port settings */
      perror("tcgetattr");
      exit(-1);
    }

    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = OPOST;

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;

    newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 1;   /* blocking read until 5 chars received */


    tcflush(fd, TCIFLUSH);

    if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }

    /*for (i = 0; i < 255; i++) {
      buf[i] = 'a';
    }*/
    
    /*testing*/
    //buf[25] = '\n';

//tp1
	/*printf("Escreva qq coisa: ");
	gets(buf);
	
    
    res = write(fd,buf,255);
	
    printf("%d bytes written\n", res);
	printf("Mensagem enviada: %s\n", buf);

	char buf2[255];	
	
	printf("antes read\n");
	read(fd, buf2, 255);
	printf("Eco: %s\n", buf2);*/
	
	
//tp2    
	set[0] = FLAG;
	set[1] = A;
	set[2] = SET_C;
	set[3] = set[1] ^ set[2];
	set[4] = FLAG;
	
	printf("SET[]: %d %d %d %d %d\n", set[0], set[1], set[2], set[3], set[4]);

	printf("WRITEE: %d\n", write(fd, set, 5));


	printf("Apos write()\n");

	alarm(3);
	
	while(flag){

	printf("depois alarm(3)\n");
	readBuffer();
	}

	printf("Depois readBuffer()\n");

	//printf("buf_test %s\n", buf_test);
	printf("Apos maquina estados read()\n");

	sleep(2); 


    if ( tcsetattr(fd,TCSANOW,&oldtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }
    close(fd);
    return 0;
}
