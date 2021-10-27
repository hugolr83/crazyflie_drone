#include "SGBA_utils.h"

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

// TODO: plug preferred orientation
SGBA_init_t getSGBAInitParam(int my_id) {
  SGBA_init_t initParam =  { .distance_from_wall = REF_DISTANCE_WALL, .max_speed = MAX_SPEED };
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

float fillHeadingArray(uint8_t *correct_heading_array, float rssi_heading, int diff_rssi, int max_meters)
{

	//Heading array of action choices
  static float heading_array[8] = { -135.0f, -90.0f, -45.0f, 0.0f, 45.0f, 90.0f, 135.0f, 180.0f};
  float rssi_heading_deg = rad2deg(rssi_heading);

  for (int it = 0; it < 8; it++) {

	  // Fill array based on heading and rssi heading
    if ((rssi_heading_deg >= heading_array[it] - 22.5f && rssi_heading_deg < heading_array[it] + 22.5f && it != 7) || (
          it == 7 && (rssi_heading_deg >= heading_array[it] - 22.5f || rssi_heading_deg < -135.0f - 22.5f))) {
      uint8_t temp_value_forward = correct_heading_array[it];
      uint8_t temp_value_backward = correct_heading_array[(it + 4) % 8];

      // if gradient is good, increment the array corresponding to the current heading and decrement the exact opposite
      if (diff_rssi > 0) {
        correct_heading_array[it] = temp_value_forward + 1; //(uint8_t)abs(diff_rssi);
        if (temp_value_backward > 0) {
          correct_heading_array[(it + 4) % 8] = temp_value_backward - 1;  //(uint8_t)abs(diff_rssi);
        }
        // if gradient is bad, decrement the array corresponding to the current heading and increment the exact opposite

      } else if (diff_rssi < 0) {
        if (temp_value_forward > 0) {
          correct_heading_array[it] = temp_value_forward - 1;  //(uint8_t)abs(diff_rssi);
        }
        correct_heading_array[(it + 4) % 8] = temp_value_backward + 1; //(uint8_t)abs(diff_rssi);
      }

    }
  }

// degrading function
  //    If one of the arrays goes over maximum amount of points (meters), then decrement all values
  if (maxValue(correct_heading_array, 8) > max_meters) {
    for (int it = 0; it < 8; it++) {
      if (correct_heading_array[it] > 0) {
        correct_heading_array[it] = correct_heading_array[it] - 1;
      }
    }
  }


  // Calculate heading where the beacon might be
  int count = 0;
  float y_part = 0, x_part = 0;

  for (int it = 0; it < 8; it++) {
    if (correct_heading_array[it] > 0) {
      x_part += (float)correct_heading_array[it] * (float)cos(heading_array[it] * (float)M_PI / 180.0f);
      y_part += (float)correct_heading_array[it] * (float)sin(heading_array[it] * (float)M_PI / 180.0f);

      //sum += heading_array[it];
      count = count + correct_heading_array[it];

    }
  }
  float wanted_angle_return = 0;
  if (count != 0) {
    wanted_angle_return = atan2(y_part / (float)count, x_part / (float)count);
  }


  return wanted_angle_return;

}