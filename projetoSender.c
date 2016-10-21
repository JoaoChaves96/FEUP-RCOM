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

#define DATA_CONTROL_FIELD	1
#define START_CONTROL_FIELD	2
#define END_CONTROL_FIELD	3

#define FLAG 0x7e

volatile int STOP=FALSE;



int main(int argc, char** argv)
{
    int fd,c, res;
    struct termios oldtio,newtio;
    char buf[255];
    int i, sum = 0, speed = 0;
    
    if ( (argc < 2) || 
  	     ((strcmp("/dev/ttyS0", argv[1])!=0) && 
  	      (strcmp("/dev/ttyS5", argv[1])!=0) )) {
      printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
      exit(1);
    }


  /*
    Open serial port device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
  */


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


	

		
}

// Provavelmente, o "octetNumber" será uma constante/variável global

/*
* Allocates memory for a package
* Leaves Data Field empty
*/
char* createDataPackage(int sequenceNumber, int octetNumber, int data) {

	unsigned int l1, l2;
	l2 = octetNumber/256;
	l1 = octetNumber%256;
	char * package = malloc(sizeof(char)*(4 + octetNumber));

	package[0] = DATA_CONTROL_FIELD;
	package[1] = sequenceNumber;
	package[2] = l2;
	package[3] = l1;
	

	return package;
}

/*
* Allocates memory for a trama
*/
char * createDataTrama() {
	char * trama = malloc(sizeof(char) * (5+octetNumber));

	trama[0] = FLAG;
	trama[1] = /*Campo de endereço*/;
	trama[2] = sequenceNumber<<7;
	trama[3] = trama[1]^trama[2];
	/*inserir pacote de dados na trama
	createDataPackage(sequenceNumber, octetNumber, data)*/
	trama[5+octetNumber-1] = FLAG;

	return trama;
	
}
