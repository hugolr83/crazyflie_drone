#include "communication_unit.h"

Command readCommand() {
    static rxPacket_t rxPacket;
    rxPacket.command = UNKNOWN_CMD;
    appchannelReceivePacket(&rxPacket, sizeof(rxPacket_t), 0);
    return rxPacket.command < UNKNOWN_CMD && rxPacket.command >= TAKE_OFF_CMD ? rxPacket.command : UNKNOWN_CMD;
}