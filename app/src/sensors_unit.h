
#include "stabilizer_types.h"
#include "estimator_kalman.h"
#include "range.h"
#include "battery.h"
#include <stdlib.h>


typedef struct range_s{
    float front;
    float back;
    float left;
    float right;
} range_t;

typedef struct sensorsData_s {
    point_t position;
    range_t range;
    int batteryLevel;
} sensorsData_t;

extern sensorsData_t sensorsData;

void updateSensorsData();
void initSensorsData();