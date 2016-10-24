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

#define ALARM_SEC 3
#define MAX_RETRIES 5
#define VALID_UA 1
#define FAILED 0
#define WAIT_ERROR -1

char * SET = {FLAG, A_SENDER, SET_CODE, A_SENDER^UA, FLAG);
char * UA = {FLAG, A_SENDER, UA_CODE, A_SENDER^UA, FLAG};

int llopen(int gate, int type);
int waitForAnswer(int fd, int sec);

/**
* Type can be 0x03 if sent by SENDER, or received by RECEIVER == type 0
* Or 0x01 if sent by RECEIVER, or received by SENDER == type 1
*/
char * createITrama(char * package, int package_length, int type);
