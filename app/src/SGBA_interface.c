/*
 * inspired from 
 *
 *  
 *      
 */


#include "SGBA_interface.h"
#include "debug.h"

//  SGBA input
static SGBA_init_t SGBA_init;
static rssi_data_t rssi_data;
static bool priority = false;


// RSSI data received from other drones/beacon
static uint8_t rssi_array_other_drones[9] = {150, 150, 150, 150, 150, 150, 150, 150, 150};
static uint64_t time_array_other_drones[9] = {0};
static float rssi_angle_array_other_drones[9] = {500.0f};

// Median filters
static struct MedianFilterFloat medFilt;

static uint8_t my_id;

// P2P communication
static P2PPacket p_reply;
static uint64_t radioSendBroadcastTime=0;

void initRSSI(){
    
    init_median_filter_f(&medFilt, 39);


    p2pRegisterCB(p2pcallbackHandler);

    uint64_t address = configblockGetRadioAddress();
    my_id =(uint8_t)((address) & 0x00000000ff);


    SGBA_init = getSGBAInitParam(my_id, REF_DISTANCE_WALL, MAX_SPEED);
    
    p_reply.port=0x00;
    p_reply.data[0]=my_id;
    p_reply.data[1]=.0f;
    p_reply.size=5;
}


void updateRSSI(){

    // For every 1 second, reset the RSSI value to high if it hasn't been received for a while
    for (uint8_t it = 0; it < 9; it++) if (usecTimestamp() >= time_array_other_drones[it] + 1000*1000) {
        time_array_other_drones[it] = usecTimestamp() + 1000*1000+1;
        rssi_array_other_drones[it] = 150;
        rssi_angle_array_other_drones[it] = 500.0f;
    }

    // get RSSI, id and angle of closests crazyflie.
    uint8_t id_inter_closest = (uint8_t)find_minimum(rssi_array_other_drones, 9);
    rssi_data.angle_inter = rssi_angle_array_other_drones[id_inter_closest]; 

    uint8_t rssi_inter_closest = rssi_array_other_drones[id_inter_closest];
    rssi_data.inter =  (uint8_t)update_median_filter_f(&medFilt, (float)rssi_inter_closest); 

    priority = id_inter_closest > my_id;
}


void initSGBA(){
    init_SGBA_controller(SGBA_init, initialPos);
}

int callSGBA(SGBA_output_t* output, bool outbound){

    orientation2d_t current_orientation = {sensorsData.position.x, sensorsData.position.y, sensorsData.yaw};
    
    //int state = SGBA_controller(output, sensorsData.range, current_orientation, rssi_data, priority, outbound);
    //DEBUG_PRINT("radio rssi = %f et angle inter = %f\n", (double)rssi_data.beacon, (double)rssi_data.angle_inter);
    int state = SGBA_controller(output, sensorsData.range, current_orientation, rssi_data, priority, outbound);

    memcpy(&p_reply.data[1], &output->rssi_angle, sizeof(float));

    // convert yaw rate commands to degrees
    output->vel_cmd.w = output->vel_cmd.w * 180.0f / (float)M_PI;
    return state;
}

void p2pcallbackHandler(P2PPacket *p){
    uint8_t id_inter_ext = p->data[0];

    uint8_t rssi_inter = p->rssi;
    float rssi_angle_inter_ext;
    memcpy(&rssi_angle_inter_ext, &p->data[1], sizeof(float));

    rssi_array_other_drones[id_inter_ext] = rssi_inter;
    time_array_other_drones[id_inter_ext] = usecTimestamp();
    rssi_angle_array_other_drones[id_inter_ext] = rssi_angle_inter_ext;  
}

void trySendBroadcast(){
    if (usecTimestamp() >= radioSendBroadcastTime + 1000*500) {
        radiolinkSendP2PPacketBroadcast(&p_reply);
        radioSendBroadcastTime = usecTimestamp();
    }
}



