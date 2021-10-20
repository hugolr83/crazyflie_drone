#ifndef SRC_BATTERY_H_
#define SRC_BATTERY_H_
#include "pm.h"
#include <stdint.h>

extern int batteryLevela;

void updateBatteryPercentage();
int fromVoltageToPercentage(double voltage);

#endif