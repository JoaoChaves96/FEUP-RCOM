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

#define SEND 0
#define RECEIVE 1

#define FLAG 0x7E
#define A_SENDER 0x03
#define A_RECEIVER 0x01
#define UA_CODE 0x07
#define SET_CODE 0x03

char * SET = {FLAG, A_SENDER, SET_CODE, A_SENDER^UA, FLAG);
char * UA = {FLAG, A_SENDER, UA_CODE, A_SENDER^UA, FLAG};

int llopen(int gate, int type);
