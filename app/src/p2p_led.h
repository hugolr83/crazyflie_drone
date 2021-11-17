#ifndef SRC_P2PLED_H_
#define SRC_P2PLED_H_


#include "sensors_unit.h"
#include "radiolink.h"
#include "led.h"
#include "utils.h"


void startFlashLedP2P();
void tryFlashLedP2P();

void onPacketP2PReceived_LED(P2PPacket *p);
void onPacketP2PSending_LED(P2PPacket *p);

#endif