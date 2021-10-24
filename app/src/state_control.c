#include "state_control.h"

stateControl_t stateControl;

void handleCommand(command_t* command){

    switch (*command)
    {
        case UNKNOWN_CMD:
            break;
        case IDENTIFY_CMD:
            flashLedApp();
            break;
        case LAND_CMD:
            stateControl.is_on_exploration_mode = false;
            stateControl.keep_flying = false;
            break;
        case START_EXPLORATION_CMD:
            stateControl.is_on_exploration_mode = true;
            stateControl.keep_flying = true;
            break;
        case TAKE_OFF_CMD:
            break;
        case RETURN_TO_BASE_CMD:
            stateControl.is_on_exploration_mode = false;
            break;
        default:
            break;
    }

    *command = UNKNOWN_CMD;
}


