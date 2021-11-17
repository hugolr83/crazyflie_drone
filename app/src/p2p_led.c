#include "p2p_led.h"

static bool started = false;
static float rssi_distance_array_other_drones[9] = {10000000.0f};

static float getMyDistance();

void startFlashLedP2P(){
    started = true;
}

void tryFlashLedP2P(){
    if(!started) return;
    uint8_t id = (uint8_t)find_minimum_f(rssi_distance_array_other_drones, 9);
    float closestOtherDistance = rssi_distance_array_other_drones[id];
    float myDistance = getMyDistance();
    if(myDistance < closestOtherDistance){
        // Turn green
        ledSet(LED_GREEN_L, 1);
    }else{
        // Turn red
        ledSet(LED_RED_L, 1);
    }
}

void onPacketP2PReceived_LED(P2PPacket* p){
    uint8_t id_inter_ext = p->data[0];

    float otherDistance;
    memcpy(&otherDistance, &p->data[2], sizeof(float));

    rssi_distance_array_other_drones[id_inter_ext] = otherDistance;
}

void onPacketP2PSending_LED(P2PPacket* p){
    float myDistance = getMyDistance();

    memcpy(&p->data[2], &myDistance, sizeof(float));
}

static float getMyDistance(){
    return sqrt(pow(sensorsData.position.x - initialPos.x, 2) + pow(sensorsData.position.y - initialPos.y, 2));
}