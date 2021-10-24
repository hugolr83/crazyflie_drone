#include "communication_unit.h"

command_t lastCommand = UNKNOWN_CMD;

command_t readCommand() {
    rxPacket_t rxPacket;
    if (appchannelReceivePacket(&rxPacket, sizeof(rxPacket_t), 0)){
        return rxPacket.command < UNKNOWN_CMD && rxPacket.command >= TAKE_OFF_CMD ? rxPacket.command : UNKNOWN_CMD;
    } else {
        return lastCommand;
    }
}