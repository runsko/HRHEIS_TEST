#include "elevator_ctrl.h"

state_t state; //MOVING_UP, MOVING_DOWN, WAIT OR EMERGENCY_STOP
direction_t direction; //keeps track of what direction we are (or last were) moving in
int currentFloor; //keeps track of what floor we are (or last were) in.
int doorOpen; //1 if door is open, 0 if door is closed

int orders[N_FLOORS][3]; //matrix of possible orders




void elevatorInitiate(){

    //initial direction
    direction = DIRN_UP;

    int floor = getFloorSensor();

    //if elevator is in floor
    if(floor != -1){
        currentFloor = floor;
        changeState(WAIT);
    }

    else{
        startMotor(direction);
        changeState(MOVING_UP);
        while(getFloorSensor() == -1){ 
            /*intentionally empty, waits until it reaches floor*/ 
        }
        stopMotor();
        changeState(WAIT);
    }
}


void changeState(state_t newstate){

    //Makes sure WAIT state does not restart timer when idle
    if(newstate == WAIT && state != WAIT && state != EMERGENCY_STOP){
        openDoor();
        stopMotor();
        startTimer();
    }
    state = newstate;
}


void openDoor(){
    if(doorOpen == 0){
        setDoorLight();
        doorOpen = 1;
    }
}
void closeDoor(){
    if(doorOpen == 1){
        resetDoorLight();
        doorOpen = 0;
    }
}


void updateFloorLight(){

    int floor = getFloorSensor();
	if (floor != -1) {
         currentFloor = floor;
    }
    setFloorIndicator(currentFloor);
}


int getOrder(button_t button, int floor){
    return orders[floor][button];
}


void addToOrders(button_t button, int floor){
    orders[floor][button] = 1;
    setButtonLamp(button, floor);
}
void removeFromOrders(button_t button, int floor){
    orders[floor][button] = 0;
    resetButtonLamp(button, floor);
}


int checkOrdersOver(){

    for (int i = currentFloor+1; i < N_FLOORS; ++i){ 
        if(getOrder(BUTTON_CALL_UP, i) || getOrder(BUTTON_COMMAND, i) || getOrder(BUTTON_CALL_DOWN, i)){
            return 1;
        }
    }
    return 0;
}

int checkOrdersUnder(){

    for (int i = currentFloor-1; i >= BOTTOM_FLOOR; --i){
        if (getOrder(BUTTON_CALL_DOWN, i) || getOrder(BUTTON_COMMAND, i) || getOrder(BUTTON_CALL_UP, i)){
            return 1;
        }
    }
    return 0;
}

void checkOrdersChangeState() {

    for (int i = 0; i < N_FLOORS; ++i){

        //executes if any button is in orders
        if (getOrder(BUTTON_CALL_UP, i) || getOrder(BUTTON_CALL_DOWN, i) || getOrder(BUTTON_COMMAND, i)) {
            if (i > currentFloor){
                changeState(MOVING_UP);
            }
            else if (i < currentFloor){
                changeState(MOVING_DOWN);
            }
            else { //handles when elevator is stopped between floors and ordered to currentFloor
                if (direction == DIRN_UP) {
                    currentFloor++; //handles bad input - consecutive stops
                    changeState(MOVING_DOWN); 
                }
                else if (direction == DIRN_DOWN) {
                    currentFloor--; //handles bad input - consecutive stops
                    changeState(MOVING_UP);
                }
            }
        }
    }
}



void checkFloorReachedUpdateOrders(){
    for (int floor = 0; floor < N_FLOORS; ++floor)
    {
        if(floor == getFloorSensor() && doorOpen){
            removeFromOrders(BUTTON_COMMAND, floor);
            if (floor != TOP_FLOOR){
                removeFromOrders(BUTTON_CALL_UP, floor);
            }
            if(floor != BOTTOM_FLOOR){
                removeFromOrders(BUTTON_CALL_DOWN, floor);
            }
        }
    }
}


void checkButtonsAddToOrders(){
    for (int floor = 0; floor < N_FLOORS; ++floor) {
        if (floor != BOTTOM_FLOOR && buttonPressed(BUTTON_CALL_DOWN, floor)){
            addToOrders(BUTTON_CALL_DOWN, floor);
        }
        if (floor != TOP_FLOOR && buttonPressed(BUTTON_CALL_UP, floor)){
            addToOrders(BUTTON_CALL_UP, floor);
        }
        if(buttonPressed(BUTTON_COMMAND, floor)){
            addToOrders(BUTTON_COMMAND, floor);
        }
    }
}


void checkIfShouldStop(){
    for(int floor = 0; floor < N_FLOORS; floor++){
        if(getFloorSensor() == floor){
            if (getOrder(BUTTON_COMMAND, floor)){
                changeState(WAIT);
            }
            else if (direction == DIRN_UP && getOrder(BUTTON_CALL_UP, floor)) {
                changeState(WAIT);
            }
            else if (direction == DIRN_DOWN && getOrder(BUTTON_CALL_DOWN, floor)) {
                changeState(WAIT);
            }
            else if (direction == DIRN_UP && getOrder(BUTTON_CALL_DOWN, floor) && !checkOrdersOver()) {
                changeState(WAIT);
            }
            else if (direction == DIRN_DOWN && getOrder(BUTTON_CALL_UP, floor) && !checkOrdersUnder()) {
                changeState(WAIT);
            }
        }
    }
}



void handleEmergencyStop(){
    int floor = getFloorSensor();
    stopMotor();
    setStopLamp();

    //deletes all orders
    for (int i = 0; i < N_FLOORS; ++i) {
        if (i != 0){
            removeFromOrders(BUTTON_CALL_DOWN, i);
        }
        if (i != N_FLOORS - 1){
            removeFromOrders(BUTTON_CALL_UP, i);
        }
        removeFromOrders(BUTTON_COMMAND, i);
    }

    if (floor != -1){
        openDoor();
        currentFloor = floor;
    }

    while(stopIsPressed()){
        
        //Keeps timer reset
        if(floor != -1){
            startTimer();
        }
    }

    resetStopLamp();
    changeState(WAIT);
}




void update() {
    if (stopIsPressed()){
        changeState(EMERGENCY_STOP);
    }
   
    checkButtonsAddToOrders();
    checkFloorReachedUpdateOrders();
    updateFloorLight();
    checkIfShouldStop();

    switch (state) {
        case MOVING_UP:
            closeDoor();
            direction = DIRN_UP;
            startMotor(direction);
            if (getFloorSensor() == TOP_FLOOR){ //safeguard
                changeState(WAIT);
            }
            break;
        case MOVING_DOWN:
            closeDoor();
            direction = DIRN_DOWN;
            startMotor(direction);
            if (getFloorSensor() == BOTTOM_FLOOR){ //safeguard
                changeState(WAIT);
            }
            break;
        case WAIT: //if elevator is waiting in a floor
            if (timerTimeOut()){ //If timer > 3
                closeDoor();
                if (direction == DIRN_UP) {
                    if (checkOrdersOver()){
                        changeState(MOVING_UP);
                    }
                    else { 
                        checkOrdersChangeState();
                    }
                }
                else if (direction == DIRN_DOWN) {
                    if (checkOrdersUnder()) {
                        changeState(MOVING_DOWN);
                    }
                    else { 
                        checkOrdersChangeState();
                    }
                }
            }
            break;
        case EMERGENCY_STOP:
            handleEmergencyStop();
            break;
    }
}

