#include "stateMachine.h"

void handleCommand(Command command){
    if (command == IDENTIFY_CMD && lastCommand != IDENTIFY_CMD) {
        flashLed();
        DEBUG_PRINT("IDENTIFY_CMD\n");
    }
    lastCommand = command;
}


