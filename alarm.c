#include "alarm.h"

void handler(int signal)
{
	if(signal != SIGALRM)
		return;

	alarmActivated = 1;

	printf("Alarm activated! \n");	
}

void setAlarm(int sec)
{
	struct sigaction action;
	action.sa_handler = handler;
	
	alarmActivated = 0;
	sigaction(SIGALRM, &action, NULL);

	alarm(sec);
	return;
}

void stopAlarm()
{
	struct sigaction action;
	action.sa_handler = NULL;
	
	sigaction(SIGALRM, &action, NULL);
	alarm(0);
	return;
}
