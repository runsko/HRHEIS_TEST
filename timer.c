#include "timer.h"

int timerActive=0;
clock_t startTime;
clock_t currentTime;
const int WAITTIME = 3;


void startTimer(){
	timerActive=1;
	startTime=clock(); //clock() returns number of clock ticks elapsed since program was lauched
	currentTime=clock();
}


int timerTimeOut(){
	if(timerActive){
		currentTime=clock();
	}
	
	if(((currentTime-startTime)/CLOCKS_PER_SEC) >= WAITTIME){
		stopTimer();
		return 1;
	}
	return 0;
}

void stopTimer(){
	timerActive=0;
}