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

// RSSI module
void initRSSI();
void updateRSSI();

// SGBA state machine module
void initSGBA();
void callSGBA(SGBA_output_t* output, bool outbound);

// radio broadcast module
void p2pcallbackHandler(P2PPacket *p);
void trySendBroadcast();