#ifndef TIMER_H
#define TIMER_H

#include <time.h>
#include <stdio.h>
#include "elevator_io.h"
#include "globals.h"


//////////////////////////////
/**This file handles timing**/
//////////////////////////////


void startTimer();
int timerTimeOut();
void stopTimer();


#endif