#ifndef SRC_SENSORSUNIT_H_
#define SRC_SENSORSUNIT_H_
#include "estimator_kalman.h"
#include "range.h"
#include "battery.h"
#include <stdlib.h>
#include <math.h>
#include "models.h"
#include "log.h"

extern sensorsData_t sensorsData;
extern point_t initialPos;

void updateSensorsData();
void storeInitialPos();

#endif
