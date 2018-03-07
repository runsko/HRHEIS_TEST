#include "timer.h"

int timerActive=0;
clock_t startClock;//LITT USIKKER PÅ CLOCK_T
clock_t currentClock;
const int WAITTIME = 1;


void startTimer(){
	timerActive=1;
	startClock=clock();
	currentClock=clock();
}


int timerTimeOut(){
	if(timerActive){
		currentClock=clock();
	}
	
	if(((currentClock-startClock)/CLOCKS_PER_SEC) >= WAITTIME){
		stopTimer();
		return 1;
	}
	return 0;
}

void stopTimer(){
	timerActive=0;
}