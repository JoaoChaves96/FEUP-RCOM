/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include "DataLink.h"
#include "ApplicationLayer.h"

volatile int STOP=FALSE;
volatile int SET_RECEIVED=FALSE;
int flag=1;

int main(int argc, char** argv)
{
	//char buf[255];
	//int fd;

	struct Application app;

	  if ( (argc < 2) ||
  	     ((strcmp("/dev/ttyS0", argv[1])!=0) &&
  	      (strcmp("/dev/ttyS1", argv[1])!=0) )) {
      printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
      exit(1);
    }

	/*fd = llopen(argv[1], RECEIVE);

	printf("file descriptor: %d\n", fd);

	close(fd);
	unsigned char s = 0;
	char oi[255];
	char buf2[10] = "1234567890";
	createPacket(oi, buf2, 10, s << 6);*/

	//llread(fd, buf);

	//write qualquer cena

	int startAppResult = startApp(&app, argv[1], RECEIVE, "file.txt", strlen("file.txt"));
	printf("main: startApp result=%d\n", startAppResult);
	int readAppResult = readApp(app);
	if(readAppResult == -1)
	{
		printf("main: readApp failed\n");
	}

	return 0;

}
