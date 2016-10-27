#include "alarm.h"


int fd;
unsigned int length;
char * buffer;

void handler() //Only calls handler when Timeout
{	
	alarmActivated = FALSE;
	if(n_timeouts >= MAX_TIMEOUTS){
		printf("connected exceeded max timeout...\n");
		exit(1);
	}
	else{
		printf("Didn´t get a response. Retrying...\n");
		retry();
		alarm(waitT);
	}

	n_timeouts++;
}

void setAlarm(int wait, int filedes, char * buf, int buf_length)
{


	buffer = buf;
	length = buf_length;
	fd = filedes;

	alarmActivated = TRUE;
	//sigaction(SIGALRM, &action, NULL);
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

void retry(){
	printf("Trying to write again...\n");
	write(fd, buffer, length);
}