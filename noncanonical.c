/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>

#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

#define FLAG 0x7e
#define A 0x03
#define SET_C 0x03
#define UA 0x07

volatile int STOP=FALSE;
volatile int SET_RECEIVED=FALSE;
int flag=1;

int main(int argc, char** argv)
{
   /* int fd,c, res;
    struct termios oldtio,newtio;
    char buf[255];
char buf2[255];

    if ( (argc < 2) || 
  	     ((strcmp("/dev/ttyS0", argv[1])!=0) && 
  	      (strcmp("/dev/ttyS1", argv[1])!=0) )) {
      printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
      exit(1);
    }


  /*
    Open serial port device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
  */
  
    
 /*   fd = open(argv[1], O_RDWR | O_NOCTTY );
    if (fd <0) {perror(argv[1]); exit(-1); }

    if ( tcgetattr(fd,&oldtio) == -1) { /* save current port settings */
    /*  perror("tcgetattr");
      exit(-1);
    }

    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
  //  newtio.c_lflag = 0;

 //   newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
   // newtio.c_cc[VMIN]     = 1;   /* blocking read until 5 chars received */



  /* 
    VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a 
    leitura do(s) próximo(s) caracter(es)
  */



/*    tcflush(fd, TCIOFLUSH);

    if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }

    printf("New termios structure set\n");
	
	
	/*Wait for the set*/
	//while()

//    while (STOP==FALSE) {       
//      res = read(fd,buf,255);   	
      
//      if (buf[res-1]=='\0')
//	 STOP=TRUE;
//	else buf[res]= '\0';
	//printf(":%s:%d\n", buf, res);
//	printf("%d %d %d %d %d", buf[0], buf[1], buf[2], buf[3], buf[4]);
	//strcat(buf2,buf);
	
	
/*	while(STOP==FALSE)
	{

		res = read(fd, buf, 255); //Le da porta serie

		if(buf[4] == FLAG)
		{
			STOP = TRUE;
			printf("%d %d %d %d %d \n", buf[0], buf[1], buf[2], buf[3], buf[4]);
		}
	}



	write(fd,buf, 5);


  /* 
    O ciclo WHILE deve ser alterado de modo a respeitar o indicado no guião 
  */



  /*  tcsetattr(fd,TCSANOW,&oldtio);
    close(fd);
    return 0;*/

	int fd, length;
	char buf[255];

	  if ( (argc < 2) ||
  	     ((strcmp("/dev/ttyS0", argv[1])!=0) &&
  	      (strcmp("/dev/ttyS1", argv[1])!=0) )) {
      printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
      exit(1);
    }

	fd = llopen(argv[1], O_RDWR | O_NOCTTY, RECEIVER);

	llread(fd, buf);
	
	//write qualquer cena

	return 0;

}
