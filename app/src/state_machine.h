#ifndef SRC_STATEMACHINE_H_
#define SRC_STATEMACHINE_H_
#include "param.h"
#include "sensors_unit.h"
#include "debug.h"
#include "state_control.h"
#include "commander_unit.h"

typedef enum
{
    NOT_READY = 0,
    READY,
    TAKING_OFF,
    LANDING,
    HOVERING,
    EXPLORATION,
    RETURNING_BASE,
    CRASHED
} state_fsm_t;


extern state_fsm_t state;



void setNextState();
void executeState();


#endif
