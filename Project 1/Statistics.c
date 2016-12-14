#include "Statistics.h"

struct Statistics stats;

void setStats(){
  stats.transmitted = 0;
  stats.received = 0;
  stats.repeated = 0;
  stats.rejected = 0;
}

void incTransmitted(){
  stats.transmitted++;
}
void incReceived(){
  stats.received++;
}
void incRepeated(){
  stats.repeated++;
}
void incRejected(){
  stats.rejected++;
}

struct Statistics getStats(){
  return stats;
}
