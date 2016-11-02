#ifndef ALARM_H
#define ALARM_H

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

#define BAUDRATE B9600
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1
#define MAX_TIMEOUTS 3

int alarmActivated;
int n_timeouts;
int waitT;

void handleAlarm();

void setAlarm(void (*func) (char *, unsigned int), char * buf, unsigned int n_length); //, int filedes, char * buf, int buf_length);

void stopAlarm();

void configureAlarm(unsigned int waitTime);

#endif
