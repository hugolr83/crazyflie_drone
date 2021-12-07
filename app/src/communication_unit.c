#include "communication_unit.h"
// Communication with server
command_t lastCommand = UNKNOWN_CMD;

// P2P communication
static P2PPacket pReply;
static uint64_t radioSendBroadcastTime=0;
static void p2pcallbackHandler(P2PPacket *p);

void readCommand() {
    rxPacket_t rxPacket;
    if (appchannelReceivePacket(&rxPacket, sizeof(rxPacket_t), 0)){
        lastCommand = rxPacket.command < UNKNOWN_CMD && rxPacket.command >= TAKE_OFF_CMD ? rxPacket.command : UNKNOWN_CMD;
    }
}


void initP2P(uint8_t myId){
    pReply.port=0x00;
    pReply.data[0]=myId;
    pReply.data[1]=.0f;// favorite angle SGBA
    pReply.data[2]=.0f;// distance from start point
    pReply.size=9;

    p2pRegisterCB(p2pcallbackHandler);
}

void trySendBroadcast(){
    // we only send packet every 500ms
    if (usecTimestamp() >= radioSendBroadcastTime + 1000*500) {

        onPacketP2PSending_SGBA(&pReply);
        onPacketP2PSending_LED(&pReply);

        radiolinkSendP2PPacketBroadcast(&pReply);
        radioSendBroadcastTime = usecTimestamp();
    }
}

static void p2pcallbackHandler(P2PPacket *p){
    onPacketP2PReceived_SGBA(p);
    onPacketP2PReceived_LED(p);
}