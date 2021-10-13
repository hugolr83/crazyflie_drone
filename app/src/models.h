#include <stdint.h>

typedef enum {
  TAKE_OFF_CMD = 0,
  LAND_CMD,
  START_EXPLORATION_CMD,
  RETURN_TO_BASE_CMD,
  IDENTIFY_CMD,
  UNKNOWN_CMD,
} Command;

typedef struct rxPacket_s {
  uint16_t command;
} __attribute__((packed)) rxPacket_t;