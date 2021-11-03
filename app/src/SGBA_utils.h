#ifndef SRC_SGBA_UTILS_H_
#define SRC_SGBA_UTILS_H_

#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "models.h"


#define MAX_RATE_SPEED 0.5f
#define SAFE_DISTANCE 0.7f
#define FORWARD_SPEED 0.3f
#define TRAVEL_DISTANCE_SAMPLE 1.0f
#define HEADING_STRATEGY_MAX_METERS 5
#define RSSI_BEACON_THRESHOLD 36

bool logicIsCloseTo(float real_value, float checked_value, float margin);
float wraptopi(float number);
void commandTurn(float *vel_w, float max_rate);
uint8_t maxValue(uint8_t myArray[], int size);
float fillHeadingArray(uint8_t *correct_heading_array, float rssi_heading, int diff_rssi, int max_meters);
int32_t find_minimum(uint8_t a[], int32_t n);
SGBA_init_t getSGBAInitParam(int my_id, float refDistanceWall, float maxSpeed);

#endif