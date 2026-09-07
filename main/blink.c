/*
 * Минимальная проверка ESP32: мигание светодиодом.
 *
 * GPIO светодиода задаётся через CONFIG_BLINK_GPIO.
 * Для типовых плат ESP32 значение по умолчанию — GPIO2,
 * но фактическую распиновку конкретной платы необходимо проверить.
 */

#include "driver/gpio.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#ifndef CONFIG_BLINK_GPIO
#define CONFIG_BLINK_GPIO 2
#endif

/**
 * Настраивает GPIO и периодически меняет уровень светодиода.
 */
void app_main(void)
{
    const gpio_num_t gpio_led = (gpio_num_t)CONFIG_BLINK_GPIO;
    int led_level = 0;

    ESP_ERROR_CHECK(gpio_set_direction(gpio_led, GPIO_MODE_OUTPUT));

    while (true) {
        led_level = !led_level;
        ESP_ERROR_CHECK(gpio_set_level(gpio_led, led_level));
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
