#ifndef SRC_SENSORSUNIT_H_
#define SRC_SENSORSUNIT_H_
#include "stabilizer_types.h"
#include "estimator_kalman.h"
#include "range.h"
#include "battery.h"
#include <stdlib.h>


typedef struct {
    float front;
    float back;
    float left;
    float right;
} range_t;

typedef struct {
    point_t position;
    range_t range;
    int batteryLevel;
} sensorsData_t;

extern sensorsData_t sensorsData;

void updateSensorsData();
void initSensorsData();

#endif