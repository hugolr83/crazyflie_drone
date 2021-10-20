#include "stateMachine.h"

void handleCommand(Command command){
    if (command == IDENTIFY_CMD && lastCommand != IDENTIFY_CMD) {
        flashLedApp();
        DEBUG_PRINT("IDENTIFY_CMD\n");
    }
    lastCommand = command;
}


