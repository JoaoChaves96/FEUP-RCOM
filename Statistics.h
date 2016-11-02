#ifndef STATISTICS_H
#define STATISTICS_H

#include <string.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>


struct Statistics{
  int transmitted;
  int received;
  int repeated;
  int rejected;
};

void setStats();

void incTransmitted();
void incReceived();
void incRepeated();
void incRejected();
struct Statistics getStats();

#endif
