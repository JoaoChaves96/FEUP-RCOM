/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include "DataLink.h"

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
	int fd, length;
	char buf[255];

	  if ( (argc < 2) ||
  	     ((strcmp("/dev/ttyS0", argv[1])!=0) &&
  	      (strcmp("/dev/ttyS1", argv[1])!=0) )) {
      printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
      exit(1);
    }

	fd = llopen(argv[1], RECEIVE);

	//llread(fd, buf);
	
	//write qualquer cena

	return 0;

}
