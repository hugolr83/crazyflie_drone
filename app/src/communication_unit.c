#include "communication_unit.h"
// Communication with server
command_t lastCommand = UNKNOWN_CMD;

// P2P communication
static P2PPacket p_reply;
static uint64_t radioSendBroadcastTime=0;
static void p2pcallbackHandler(P2PPacket *p);

void readCommand() {
    rxPacket_t rxPacket;
    if (appchannelReceivePacket(&rxPacket, sizeof(rxPacket_t), 0)){
        lastCommand = rxPacket.command < UNKNOWN_CMD && rxPacket.command >= TAKE_OFF_CMD ? rxPacket.command : UNKNOWN_CMD;
    }
}


void initP2P(int myId){
    p_reply.port=0x00;
    p_reply.data[0]=myId;
    p_reply.data[1]=.0f;// favorite angle SGBA
    p_reply.data[2]=.0f;// distance from start point
    p_reply.size=5;

    p2pRegisterCB(p2pcallbackHandler);
}

void trySendBroadcast(){
    // we only send packet every 500ms
    if (usecTimestamp() >= radioSendBroadcastTime + 1000*500) {

        onPacketP2PSending_SGBA(&p_reply);

        float distance = sqrt(pow(sensorsData.position.x - initialPos.x, 2) + pow(sensorsData.position.y - initialPos.y, 2));

        memcpy(&p_reply.data[2], &distance, sizeof(float));

        radiolinkSendP2PPacketBroadcast(&p_reply);
        radioSendBroadcastTime = usecTimestamp();
    }
}

static void p2pcallbackHandler(P2PPacket *p){
    onPacketP2PReceived_SGBA(p);
    // TODO recieve other drone distance here and activate led
}