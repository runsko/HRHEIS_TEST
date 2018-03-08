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

///////////////////////////comments done to here/////////////////////////////////////


void checkOrdersChangeState() {

    for (int i = 0; i < N_FLOORS; ++i){

        //executes if any type of button of a floor is ordered
        if (getOrder(BUTTON_CALL_UP, i) || getOrder(BUTTON_CALL_DOWN, i) || getOrder(BUTTON_COMMAND, i)) {
            if (i > currentFloor){
                changeState(MOVING_UP);
            }
            else if (i < currentFloor){
                changeState(MOVING_DOWN);
            }
            else { //handles the case where elevator was stopped between floors and is ordered to currentFloor
                if (direction == DIRN_UP) { 
                    currentFloor++; //prevents the change of direction when elevator is stopped consecutively between floors - special rare occasion
                    changeState(MOVING_DOWN); 
                }
                else if (direction == DIRN_DOWN) {
                    currentFloor--; //prevents the change of direction when elevator is stopped consecutively between floors - special rare occasion
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
    for (int floor = 0; floor < N_FLOORS; ++floor)
    {
        if (floor != 0 && buttonPressed(BUTTON_CALL_DOWN, floor)){
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
            if (direction == DIRN_UP && getOrder(BUTTON_CALL_UP, floor)) {
                changeState(WAIT);
            }
            if (direction == DIRN_DOWN && getOrder(BUTTON_CALL_DOWN, floor)) {
                changeState(WAIT);
            }
            if (direction == DIRN_UP && getOrder(BUTTON_CALL_DOWN, floor) && !checkOrdersOver()) {
                changeState(WAIT);
            }
            if (direction == DIRN_DOWN && getOrder(BUTTON_CALL_UP, floor) && !checkOrdersUnder()) {
                changeState(WAIT);
            }
        }
    }
}



void handleEmergencyStop(){
    //stops elevator
    stopMotor();


//if elevator is in floor when pressed: 
//  *the door is opened
//  *stays open three seconds after button is released

    //deletes all orders in Orders
    for (int i = 0; i < N_FLOORS; ++i) {
        if (i != 0)
            removeFromOrders(BUTTON_CALL_DOWN, i);

        if (i != N_FLOORS - 1)
            removeFromOrders(BUTTON_CALL_UP, i);

        removeFromOrders(BUTTON_COMMAND, i);
    }

    //runs while button is pressed (so no orders can come in)
    while(stopIsPressed()){
        setStopLamp();
        
        //if elevator is in floor while pressed. Keeps door open and timer reset
        if(getFloorSensor() != -1){
            openDoor();
            startTimer();
        }
    }

    resetStopLamp();

    //if elevator is in floor when pressed: the door is opened
    int floor = getFloorSensor();
    if(floor != -1){
        currentFloor = floor;
        if(timerTimeOut()){ //Hvis timer > 3
            closeDoor();
        }
    }
    changeState(WAIT);
    
    
}
/////////////////////////////////



void update() {
    if(stopIsPressed()){
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
            if(getFloorSensor() == TOP_FLOOR){
                changeState(WAIT);
            }
            break;
        case MOVING_DOWN:
            closeDoor();
            direction = DIRN_DOWN;
            startMotor(direction);
            if(getFloorSensor() == BOTTOM_FLOOR){
                changeState(WAIT);
            }
            break;
        case WAIT: //if elevator is waiting in a floor
            
            if(timerTimeOut()){ //If timer > 3
                closeDoor();
                if (direction == DIRN_UP) {
                    if (!checkOrdersOver()){
                        checkOrdersChangeState();
                    }
                    else { changeState(MOVING_UP);}
                }
                else if (direction == DIRN_DOWN) {
                    if (!checkOrdersUnder()) {
                        checkOrdersChangeState();
                    }
                    else { changeState(MOVING_DOWN);}
                }
            }
            break;
        case EMERGENCY_STOP:
            handleEmergencyStop();
            break;
    }
}

