#ifndef SRC_COMMUNICATION_H_
#define SRC_COMMUNICATION_H_
#include "models.h"
#include "app_channel.h"
#include "radiolink.h"
#include "SGBA_interface.h"
#include "p2p_led.h"


extern command_t lastCommand;

void readCommand();
void initP2P(uint8_t myId);

// radio broadcast module
void trySendBroadcast();

#endif