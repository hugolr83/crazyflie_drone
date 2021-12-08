#ifndef SRC_MODELS_H_
#define SRC_MODELS_H_
#include <stdint.h>
#include "stabilizer_types.h"


typedef enum {
  TAKE_OFF_CMD = 0,
  LAND_CMD,
  START_EXPLORATION_CMD,
  RETURN_TO_BASE_CMD,
  IDENTIFY_CMD,
  P2P_CMD,
  UNKNOWN_CMD,
} command_t;

typedef struct {
  uint16_t command;
} __attribute__((packed)) rxPacket_t;


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
    float yaw; // rad
    float voltageFiltered;
} sensorsData_t;


// SGBA models

typedef struct vec2d_angle_s {
    float x;
    float y;
    float w;
} vec2d_angle_t;

typedef struct vec2d_angle_s orientation2d_t;
typedef struct vec2d_angle_s velocity2d_t;

typedef struct {
    float distance_from_wall;
    float max_speed;
    float wanted_angle;
} SGBA_init_t;

typedef struct {
  velocity2d_t vel_cmd;
  float rssi_angle;
  int state_wallfollowing;
} SGBA_output_t;

typedef struct {
  uint8_t inter;
  float angle_inter;
} rssi_data_t;

#endif