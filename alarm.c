#include "alarm.h"

int fd;
unsigned int length;
char * buffer;

void handler() //Only calls handler when Timeout
{
	alarmActivated = FALSE;
	if(n_timeouts >= MAX_TIMEOUTS){
		printf("connected exceeded max timeout...\n");
		//exit(1);
		alarmActivated = TRUE;
	}
	else{
		printf("Didn´t get a response. Retrying...\n");
		alarm(waitT);
	}

	n_timeouts++;
}

void setAlarm(int wait)
{

/*
	buffer = buf;
	length = buf_length;
	fd = filedes;
*/
	alarmActivated = FALSE;
	n_timeouts = 0;
	waitT = wait;
	alarm(waitT);
	return;
}

void stopAlarm()
{
	alarmActivated = FALSE;
	alarm(0);
}

void configureAlarm()
{
	(void) signal(SIGALRM, handler);
	n_timeouts = 0;
}

int getN_timouts(){
	return n_timeouts;
}
/*
void retry(){
	printf("Trying to write again...\n");
	write(fd, buffer, length);
}
*/
