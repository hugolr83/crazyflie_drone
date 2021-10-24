
#ifndef SRC_COMMANDERUNIT_H_
#define SRC_COMMANDERUNIT_H_
#include "commander.h"
#include "stabilizer_types.h"

extern setpoint_t setpoint;

void take_off(setpoint_t *sp, float velocity);
void land(setpoint_t *sp, float velocity);
void hover(setpoint_t *sp, float height);
void vel_command(setpoint_t *sp, float vel_x, float vel_y, float yaw_rate, float height);
void shut_off_engines(setpoint_t *sp);

#endif