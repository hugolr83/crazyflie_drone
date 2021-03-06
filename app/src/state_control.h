#ifndef SRC_STATECONTROL_H_
#define SRC_STATECONTROL_H_
#include "models.h"
#include "led_app.h"
#include "commander_unit.h"
#include "p2p_led.h"


typedef struct
{
  bool is_on_exploration_mode;
} state_control_t;

extern state_control_t stateControl;

void handleCommand(command_t* command);



#endif
