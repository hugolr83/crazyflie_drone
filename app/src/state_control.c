#include "state_control.h"
#include "state_machine.h"

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
            }
            break;
        case START_EXPLORATION_CMD:
            if(state == READY) {
                state = TAKING_OFF;
                storeInitialPos();// store initial position on start of mission
            }
            break;
        case TAKE_OFF_CMD:
            // don't do shit
            break;
        case RETURN_TO_BASE_CMD:
            if(state == HOVERING){
                state = LANDING;
            }
            else if(state == EXPLORATION){
                state = RETURNING_BASE;
            }
            break;
        default:
            break;
    }

    *command = UNKNOWN_CMD;
}


