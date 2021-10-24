#ifndef SRC_LEDAPP_H_
#define SRC_LEDAPP_H_

#include "FreeRTOS.h"
#include "led.h"
#include "task.h"
#include "debug.h"

static const unsigned NUMBER_OF_FLASH = 10;
static const TickType_t FLASH_DELAY = 250;

void flashLedApp();

#endif
