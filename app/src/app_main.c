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

#include "FreeRTOS.h"
#include "task.h"

#include "debug.h"

#include "ledseq.h"
#include "crtp_commander_high_level.h"
#include "locodeck.h"
#include "mem.h"
#include "log.h"
#include "param.h"
#include "pm.h"
#include "system.h"
#include "communication_unit.h"


static const unsigned NUMBER_OF_FLASH = 1;
static const TickType_t FLASH_DELAY = 250;

void flashLed() {
    // This would need to be moved to its own low priority task that would block on notification from the other
    // task, but it is not needed for now
    for (unsigned i = 0; i < NUMBER_OF_FLASH; i++) {
        ledSetAll();
        vTaskDelay(FLASH_DELAY);
        ledClearAll();
        vTaskDelay(FLASH_DELAY);
    }
}



_Noreturn void appMain() {
  while (true) {
    Command command = readCommand();
    if (command == IDENTIFY_CMD) {
        flashLed();
    }
  }  
}