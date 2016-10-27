#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "alarm.h"

#define BAUDRATE B9600
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

#define SEND 0
#define RECEIVE 1

#define FLAG 0x7E
#define A_SENDER 0x03
#define A_RECEIVER 0x01
#define UA_CODE 0x07
#define SET_CODE 0x03
#define DISC_CODE 0x0B
#define RR_0 0x05
#define RR_1 0x85
#define REJ_0 0x01
#define REJ_1 0x81

#define PACKET_LENGTH 5

#define ALARM_SEC 3
#define MAX_RETRIES 3
#define VALID_COMMAND 1
#define FAILED 0
#define WAIT_ERROR -1

int llopen(char * path, int type);

int llwrite(int fd, char * buffer, int length);

int llread(int fd, char* trama);

int llclose(int fd, int programType);