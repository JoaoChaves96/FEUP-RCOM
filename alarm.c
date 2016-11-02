#include "alarm.h"

void (* handler_func) (char *, unsigned int);

unsigned int length;
char * buffer;

void handleAlarm() //Only calls handler when Timeout
{
	/*alarmActivated = FALSE;
	if(n_timeouts >= MAX_TIMEOUTS){
		printf("connected exceeded max timeout...\n");
		//exit(1);
		alarmActivated = TRUE;
	}
	else{
		printf("Didn´t get a response. Retrying...\n");
		alarm(waitT);
	}

	n_timeouts++;*/

	if(n_timeouts<= MAX_TIMEOUTS){
		printf("Didn´t get a response. Retrying...\n");
		handler_func(buffer, length);
		alarm(waitT);
	}
	else{
		alarmActivated= TRUE;
		alarm(0);
	}
	n_timeouts++;
	return;
}

void setAlarm(void (*func) (char *, unsigned int), char* buf, unsigned int n_length)
{

/*
	buffer = buf;
	length = buf_length;
	fd = filedes;
*/
/*	alarmActivated = FALSE;
	n_timeouts = 0;
	waitT = wait;
	alarm(waitT);
	return;*/

handler_func=func;
buffer=buf;
length=n_length;

n_timeouts=1;
alarmActivated= FALSE;
alarm(waitT);

}

void stopAlarm()
{
	//alarmActivated = FALSE;
	alarm(0);
}

void configureAlarm(unsigned int waitTime)
{
/*	struct sigaction interruptAction;
	interruptAction.sa_handler = handler;
	interruptAction.sa_flags &= !SA_RESTART;
	sigaction(SIGALRM, &interruptAction, NULL);
	//(void) signal(SIGALRM, handler);
	n_timeouts = 0;*/

	(void) signal(SIGALRM, handleAlarm);
	waitT= waitTime;
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
