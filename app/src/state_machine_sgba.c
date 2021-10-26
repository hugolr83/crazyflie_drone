/*
 * gradient_bug.c
 *
 *  Created on: Aug 9, 2018
 *      Author: knmcguire
 */


#include <string.h>
#include <errno.h>
#include <math.h>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "log.h"
#include "param.h"
#include "system.h"

#include "commander.h"
#include "sensors.h"
#include "stabilizer_types.h"

#include "estimator_kalman.h"
#include "stabilizer.h"

#include "wallfollowing_multiranger_onboard.h"
#include "SGBA.h"
#include "usec_time.h"
#include "models.h"


#include "range.h"
#include "radiolink.h"
#include "median_filter.h"
#include "configblock.h"

#include "SGBA_utils.h"



void p2pcallbackHandler(P2PPacket *p);


static uint8_t rssi_inter_filtered;
static uint8_t rssi_beacon_filtered;
float rssi_angle_inter_closest;

static uint8_t rssi_beacon;

static uint8_t rssi_array_other_drones[9] = {150, 150, 150, 150, 150, 150, 150, 150, 150};
static uint64_t time_array_other_drones[9] = {0};
static float rssi_angle_array_other_drones[9] = {500.0f};

static uint8_t id_inter_closest=100;

static struct MedianFilterFloat medFilt;
static struct MedianFilterFloat medFilt_2;
static struct MedianFilterFloat medFilt_3;
static uint8_t my_id;
static P2PPacket p_reply;


static uint64_t radioSendBroadcastTime=0;
// static uint64_t takeoffdelaytime = 0;

void initOnlyOneTime(){
    
    init_median_filter_f(&medFilt, 5);
    
    init_median_filter_f(&medFilt_2, 5);
    
    init_median_filter_f(&medFilt_3, 13);


    p2pRegisterCB(p2pcallbackHandler);

    uint64_t address = configblockGetRadioAddress();
    my_id =(uint8_t)((address) & 0x00000000ff);
    
    p_reply.port=0x00;
    p_reply.data[0]=my_id;
    p_reply.data[1]=.0f;
    p_reply.size=5;
}

void readEveryStep(){

    // For every 1 second, reset the RSSI value to high if it hasn't been received for a while
    for (uint8_t it = 0; it < 9; it++) if (usecTimestamp() >= time_array_other_drones[it] + 1000*1000) {
        time_array_other_drones[it] = usecTimestamp() + 1000*1000+1;
        rssi_array_other_drones[it] = 150;
        rssi_angle_array_other_drones[it] = 500.0f;
    }

    // get RSSI, id and angle of closests crazyflie.
    id_inter_closest = (uint8_t)find_minimum(rssi_array_other_drones, 9);
    rssi_angle_inter_closest = rssi_angle_array_other_drones[id_inter_closest]; //

    uint8_t rssi_inter_closest = rssi_array_other_drones[id_inter_closest];
    rssi_inter_filtered =  (uint8_t)update_median_filter_f(&medFilt_2, (float)rssi_inter_closest); //


    //t RSSI of beacon
    rssi_beacon_filtered =  (uint8_t)update_median_filter_f(&medFilt_3, (float)rssi_beacon); //

}


void initSGBA(){
    if (my_id == 4 || my_id == 8) {
        init_SGBA_controller(0.4, 0.5, -0.8);
    } else if (my_id == 2 || my_id == 6) {
        init_SGBA_controller(0.4, 0.5, 0.8);
    } else if (my_id == 3 || my_id == 7) {
        init_SGBA_controller(0.4, 0.5, -2.4);
    } else if (my_id == 5 || my_id == 9) {
        init_SGBA_controller(0.4, 0.5, 2.4);
    } else {
        init_SGBA_controller(0.4, 0.5, 0.8);
    }
}


void callSGBA(SGBA_output_t* output, bool outbound){

    bool priority = false;
    if (id_inter_closest > my_id) {
        priority = true;
    } else {
        priority = false;

    }

    rssi_data_t rssi_data;

    //TODO make outbound depended on battery.
    state = SGBA_controller(output, , heading_rad,
                                            (float)pos.x, (float)pos.y, rssi_beacon_filtered, rssi_inter_filtered, rssi_angle_inter_closest, priority, outbound);

    memcpy(&p_reply.data[1],&rssi_angle, sizeof(float));

    // convert yaw rate commands to degrees
    float vel_w_cmd_convert = vel_w_cmd * 180.0f / (float)M_PI;
    vel_command(&setpoint_BG, vel_x_cmd, vel_y_cmd, vel_w_cmd_convert, nominal_height);
    on_the_ground = false;
}


void endStep(){

    if (usecTimestamp() >= radioSendBroadcastTime + 1000*500) {
        radiolinkSendP2PPacketBroadcast(&p_reply);
        radioSendBroadcastTime = usecTimestamp();
    }

}


void p2pcallbackHandler(P2PPacket *p)
{
    uint8_t id_inter_ext = p->data[0];


    if(id_inter_ext == 0x64){
        rssi_beacon =p->rssi;
    }
    else{
        uint8_t rssi_inter = p->rssi;
        float rssi_angle_inter_ext;
        memcpy(&rssi_angle_inter_ext, &p->data[1], sizeof(float));

        rssi_array_other_drones[id_inter_ext] = rssi_inter;
        time_array_other_drones[id_inter_ext] = usecTimestamp();
        rssi_angle_array_other_drones[id_inter_ext] = rssi_angle_inter_ext;
    }
}

