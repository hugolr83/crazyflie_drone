#include "utils.h"

// Converts degrees to radians.
#define deg2rad(angleDegrees) (angleDegrees * (float)M_PI / 180.0f)

// Converts radians to degrees.
#define rad2deg(angleRadians) (angleRadians * 180.0f / (float)M_PI)


// Static helper functions
bool logicIsCloseTo(float real_value, float checked_value, float margin)
{
  if (real_value > checked_value - margin && real_value < checked_value + margin) {
    return true;
  } else {
    return false;
  }
}

float wraptopi(float number)
{
  if (number > (float)M_PI) {
    return (number - (float)(2 * M_PI));
  } else if (number < (float)(-1 * M_PI)) {
    return (number + (float)(2 * M_PI));
  } else {
    return (number);
  }

}


// Command functions
void commandTurn(float *vel_w, float max_rate)
{
  *vel_w = max_rate;
}

uint8_t maxValue(uint8_t myArray[], int size)
{
  /* enforce the contract */
  //assert(myArray && size);
  int i;
  uint8_t maxValue = myArray[0];

  for (i = 1; i < size; ++i) {
    if (myArray[i] > maxValue) {
      maxValue = myArray[i];
    }
  }
  return maxValue;
}

int32_t find_minimum(uint8_t a[], int32_t n)
{
  int32_t c, min, index;

  min = a[0];
  index = 0;

  for (c = 1; c < n; c++) {
    if (a[c] < min) {
      index = c;
      min = a[c];
    }
  }

  return index;
}

int32_t find_minimum_f(float a[], int32_t n)
{
  int32_t c, min, index;

  min = a[0];
  index = 0;

  for (c = 1; c < n; c++) {
    if (a[c] < min) {
      index = c;
      min = a[c];
    }
  }

  return index;
}

// TODO: plug preferred orientation here
SGBA_init_t getSGBAInitParam(int my_id, float refDistanceWall, float maxSpeed) {
  SGBA_init_t initParam =  { .distance_from_wall = refDistanceWall, .max_speed = maxSpeed };
  float wanted_angle = 0;
  if (my_id == 4 || my_id == 8) {
      wanted_angle = -0.8;
  } else if (my_id == 2 || my_id == 6) {
      wanted_angle = -0.8;
  } else if (my_id == 3 || my_id == 7) {
      wanted_angle = -2.4;
  } else if (my_id == 5 || my_id == 9) {
      wanted_angle = 2.4;
  } else {
      wanted_angle = 0.8;
  }
  initParam.wanted_angle = wanted_angle;
  return initParam;
}