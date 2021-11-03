#ifndef SRC_BATTERY_H_
#define SRC_BATTERY_H_
#include "pm.h"
#include <stdint.h>
#include "supervisor.h"
#include "sensors_unit.h"

void initBattery();
float updateBatteryVoltage();
int getBatteryPercentage();


#endif