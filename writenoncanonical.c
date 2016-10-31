/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include "DataLink.h"
#include "ApplicationLayer.h"

int main(int argc, char** argv)
{
  struct Application app;

    if ( (argc < 2) ||
  	     ((strcmp("/dev/ttyS0", argv[1])!=0) &&
  	      (strcmp("/dev/ttyS1", argv[1])!=0) )) {
      printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
      exit(1);
    }

	//int startAppResult = startApp(&app, argv[1], SEND, "pinguim.gif",11);
  int startAppResult = startApp(&app, argv[1], SEND, "test.txt", strlen("test.txt") + 1);
  printf("main: startApp result=%d", startAppResult);
  writeApp(app);

	return 0;

  /*char package[5];
  package[0] = FLAG;
  package[1] = A_SENDER;
  package[2] = 3;
  package[3] = (package[1]^package[2]);
  package[4] = FLAG;

  validPacket(package);

  package[2] = 5;

  validPacket(package);

  package[2] = 133;

  validPacket(package);*/

}
