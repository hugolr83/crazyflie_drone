/**
 * ,---------,       ____  _ __
 * |  ,-^-,  |      / __ )(_) /_______________ _____  ___
 * | (  O  ) |     / __  / / __/ ___/ ___/ __ `/_  / / _ \
 * | / ,--´  |    / /_/ / / /_/ /__/ /  / /_/ / / /_/  __/
 *    +------`   /_____/_/\__/\___/_/   \__,_/ /___/\___/
 *
 * Crazyflie control firmware
 *
 * Copyright (C) 2020 Bitcraze AB
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, in version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * api_app.c - App layer application that calls app API functions to make
 *             sure they are compiled in CI.
 */


#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "app.h"
#include "log.h"
#include "sensors_unit.h"
#include "state_control.h"
#include "communication_unit.h"
#include "state_machine.h"
#include "SGBA_interface.h"
#include "usec_time.h"
#include "radiolink.h"
#include "configblock.h"
#include "p2p_led.h"
#include "system.h"

#define STATE_MACHINE_COMMANDER_PRI 3



void appMain() {

  uint64_t address = configblockGetRadioAddress();
  uint8_t myId =(uint8_t)((address) & 0x00000000ff);

  initSGBAModule(myId);
  initP2P(myId);
  initBattery();

  systemWaitStart();
  vTaskDelay(M2T(3000));
  
  while (true) {
    vTaskDelay(10);

    readCommand();

    tryFlashLedP2P();

    updateSensorsData();
    updateSGBAModule();
    memset(&setpoint, 0, sizeof(setpoint));
    
    handleCommand(&lastCommand);
    
    stateMachineStep();

    commanderSetSetpoint(&setpoint, STATE_MACHINE_COMMANDER_PRI);
  }  
  
}

LOG_GROUP_START(drone)
LOG_ADD(LOG_UINT8, state, &state)
LOG_ADD(LOG_FLOAT, batteryFiltered, &sensorsData.voltageFiltered)
LOG_ADD(LOG_UINT8, batteryLevel, &sensorsData.batteryLevel)
LOG_GROUP_STOP(drone)

