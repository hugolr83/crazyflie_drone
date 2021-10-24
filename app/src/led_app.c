#include "led_app.h"

void flashLedApp() {
    // This would need to be moved to its own low priority task that would block on notification from the other
    // task, but it is not needed for now
    for (unsigned i = 0; i < NUMBER_OF_FLASH; i++) {
        ledSetAll();
        vTaskDelay(FLASH_DELAY);
        ledClearAll();
        vTaskDelay(FLASH_DELAY);
    }
}