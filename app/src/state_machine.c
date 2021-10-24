#include "stateMachine.h"






void handleCommand(Command command, Command lastCommand){
    if (command == IDENTIFY_CMD && lastCommand != IDENTIFY_CMD) {
        flashLedApp();
        DEBUG_PRINT("IDENTIFY_CMD\n");
    }
    if (command == TAKE_OFF_CMD){
        take_off(&setpoint, 0.5);
        DEBUG_PRINT("TAKE_OFF_CMD\n");
    }
}


