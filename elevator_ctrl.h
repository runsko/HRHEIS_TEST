#ifndef ELEVATOR_CTRL_H
#define ELEVATOR_CTRL_H

#include "elevator_io.h"
#include "timer.h"
#include "globals.h"
#include <stdio.h>



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
int getOrders(button_t button, int floor);


//adds or removes an order from the Orders
//sets or resets button light
void addToOrders(button_t button, int floor);
void removeFromOrders(button_t button, int floor);


//returns 1 if there are unhandled orders above/below the elevator, 0 otherwise
int checkUpwards();
int checkDownwards();


///////////////////////////comments done to here/////////////////////////////////////


//checks if any order is present and changes state accordingly
void checkAllButtons();

//Checks if any of the ordering buttons are pressed and adds the order to Orders if they are
void checkButtonsAddToOrders();

//removes the correct btns from Orders and turns off btn light if the elevator reaches the floor and door opens
void checkFloorReachedUpdateOrders();


//checks if the elevator should stop
//Stops if the floor is ordered by COMMAND button
void checkIfShouldStop();



//handles what happens when stop button is pressed
void handleEmergencyStop();




//HAS MAIN FUNCTIONALITY
//checks if buttons are pressed
//checks if a floor is reached and removes it from the Orders
//chechs if elevator should stop at floor
//updates the floor light
//Tells elevator what to do depending on which STATE it is in
void update();



#endif