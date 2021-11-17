#ifndef SRC_COMMUNICATION_H_
#define SRC_COMMUNICATION_H_
#include "models.h"
#include "app_channel.h"
#include "radiolink.h"
#include "SGBA_interface.h"


extern command_t lastCommand;

void readCommand();
void initP2P(int myId);

// radio broadcast module
void trySendBroadcast();

#endif