#ifndef SRC_SGBA_INTERFACE_H_
#define SRC_SGBA_INTERFACE_H_

#include <string.h>
#include <errno.h>
#include <math.h>

#include "usec_time.h"
#include "radiolink.h"
#include "configblock.h"

#include "models.h"
#include "sensors_unit.h"
#include "SGBA.h"
#include "median_filter.h"

#define REF_DISTANCE_WALL 0.4f // m
#define MAX_SPEED 0.5f // m/s

// RSSI module
void initRSSI();
void updateRSSI();

// SGBA state machine module
void initSGBA();
int callSGBA(SGBA_output_t* output, bool outbound);

// radio broadcast module
void p2pcallbackHandler(P2PPacket *p);
void trySendBroadcast();

#endif