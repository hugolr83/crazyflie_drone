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
} sensorsData_t;

#endif