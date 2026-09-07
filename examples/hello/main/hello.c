/*
 * Третий тест ESP32: проверка UART-монитора.
 *
 * В мониторе каждые две секунды появляется строка HELLO ESP32.
 */

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "hello";

/** Выводит приветствие в UART-монитор ESP-IDF. */
void app_main(void)
{
    ESP_LOGI(TAG, "HELLO ESP32");

    while (true) {
        vTaskDelay(pdMS_TO_TICKS(2000));
        ESP_LOGI(TAG, "HELLO ESP32");
    }
}
