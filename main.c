#include "elev.h"
#include "elevator_ctrl.h"
#include <stdio.h>


int main() {
    // Initialize hardware
    if (!elev_init()) {
        printf("Unable to initialize elevator hardware!\n");
        return 1;
    }

    printf("Press STOP button to stop elevator and exit program.\n");

    elevatorInitiate();

    while (1) {

        update();

        // Stop elevator and exit program if the stop button is pressed
        /*if (elev_get_stop_signal()) {
            elev_set_motor_direction(DIRN_STOP);
            break;
        }*/
    }

    return 0;
}
