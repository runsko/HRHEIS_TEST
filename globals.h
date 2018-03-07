#ifndef GLOBALS_H
#define GLOBALS_H

#include "elev.h"

#define TOP_FLOOR N_FLOORS-1
#define BOTTOM_FLOOR 0

typedef enum {
    MOVING_UP, 
    MOVING_DOWN, 
    WAIT, 
    EMERGENCY_STOP
} state_t;


#define direction_t elev_motor_direction_t

#define button_t elev_button_type_t



#endif