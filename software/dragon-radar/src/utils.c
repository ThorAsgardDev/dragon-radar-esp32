
#include "utils.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"


void delay(uint32_t ms) {
    vTaskDelay(ms / portTICK_PERIOD_MS);
}

int64_t millis(void) {
    return esp_timer_get_time() / 1000;
}
