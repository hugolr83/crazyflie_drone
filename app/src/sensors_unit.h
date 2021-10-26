#ifndef SRC_SENSORSUNIT_H_
#define SRC_SENSORSUNIT_H_
#include "estimator_kalman.h"
#include "range.h"
#include "battery.h"
#include <stdlib.h>
#include "models.h"

extern sensorsData_t sensorsData;

void updateSensorsData();

#endif
