#ifndef SRC_BATTERY_H_
#define SRC_BATTERY_H_
#include "pm.h"
#include <stdint.h>

int getBatteryPercentage();
int fromVoltageToPercentage(double voltage);

#endif