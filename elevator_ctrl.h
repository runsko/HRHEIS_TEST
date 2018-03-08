#ifndef ELEVATOR_CTRL_H
#define ELEVATOR_CTRL_H

#include "elevator_io.h"
#include "timer.h"
#include "globals.h"
#include <stdio.h>


/////////////////////////////////////
/**This file controls the elevator**/
/////////////////////////////////////




//initiates the elevator to go UP to the nearest floor (when program starts)
//sets state to WAIT when floor is reached
void elevatorInitiate();

//function to handle changing of states, accommodates previous state
void changeState(state_t newstate);


//Opens/Closes the door and updates variable doorOpen
void openDoor();
void closeDoor();


//updates currentFloor if floor is reached
//sets floor indicator lamp of the currentFloor (or last known)
void updateFloorLight();


//Returns 1 if button is present in Orders, 0 if not
int getOrder(button_t button, int floor);


//adds or removes an order from the Orders
//sets or resets button light
void addToOrders(button_t button, int floor);
void removeFromOrders(button_t button, int floor);


//returns 1 if there are unhandled orders above/below the elevator, 0 otherwise
int checkOrdersOver();
int checkOrdersUnder();


//checks if any order is present and changes state accordingly
void checkOrdersChangeState();

//Checks if any of the buttons are pressed and updates orders
void checkButtonsAddToOrders();

//removes the correct buttons from orders
//turns off button light if the elevator reaches the floor and the door is open
void checkFloorReachedUpdateOrders();


//checks if the elevator should stop at the floor it's passing
void checkIfShouldStop();



//handles what happens when stop button is pressed
//stops, deletes orders
//opens door and starts timer when button released (if in floor)
void handleEmergencyStop();




//HAS MAIN FUNCTIONALITY
//Checks if buttons are pressed and adds to orders
//Checks if a floor is reached and removes it from the Orders
//Checks if elevator should stop at the floor it passes
//Updates the floor light
//Tells elevator what to do depending on which STATE it is in
void update();



#endif