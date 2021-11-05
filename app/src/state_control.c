#include "state_control.h"
#include "state_machine.h"
state_control_t stateControl;

void handleCommand(command_t* command){

    switch (*command)
    {
        case UNKNOWN_CMD:
            break;
        case IDENTIFY_CMD:
            flashLedApp();
            break;
        case LAND_CMD:
            if(state == HOVERING || state == EXPLORATION || state == RETURNING_BASE){
                state = LANDING;
                stateControl.is_on_exploration_mode = false;
            }
            break;
            
        case START_EXPLORATION_CMD:
            if(state == READY) {
                stateControl.is_on_exploration_mode = true;
            }
            break;

        case TAKE_OFF_CMD:
            break;

        case RETURN_TO_BASE_CMD:
            if(state == HOVERING){
                state = LANDING;
            }
            else if(state == EXPLORATION){
                state = RETURNING_BASE;
                stateControl.is_on_exploration_mode = false;
            }
            break;

        default:
            break;
    }

    *command = UNKNOWN_CMD;
}


