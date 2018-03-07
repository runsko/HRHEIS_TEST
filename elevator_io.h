#ifndef ELEVATOR_IO_H
#define ELEVATOR_IO_H

#include "elev.h"
#include "globals.h"

//starts motor in direction <dir>
void startMotor(direction_t dir);
void stopMotor();


void setDoorLight();
void resetDoorLight();

//returns -1 if elevator is not on a floor. 0-3 if elevator is on floor.
int getFloorSensor();


//Returns 0 if button is not pressed, 1 if button is pressed
//floor 0..3
int buttonPressed(button_t button, int floor);

//Returns 0 if stop button is not pressed, 1 if stop button is pressed
int stopIsPressed();



//makes a buttons lamp light up
void setButtonLamp(button_t button, int floor);
//turns off a buttons lamp light
void resetButtonLamp(button_t button, int floor);


//makes the stop lamp light up
void setStopLamp();
//turns off the stop lamp light
void resetStopLamp();


//sets the floor indicator of the floor <currFloor>
//Other floor lamps are turned off.
void setFloorIndicator(int currFloor);

#endif