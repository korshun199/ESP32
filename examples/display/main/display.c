/*
 * Тест встроенного OLED 128x64 на плате ESP32-WROOM-32.
 *
 * Дисплей: SSD1306, I2C 0x3C.
 * SDA: GPIO21. SCL: GPIO22.
 * Цвет строк задаётся фиксированными зонами самого OLED-модуля.
 */

#include <stddef.h>
#include <stdint.h>

#include "driver/i2c.h"
#include "esp_err.h"
#include "esp_log.h"

#define OLED_I2C_PORT I2C_NUM_0
#define OLED_I2C_SDA GPIO_NUM_21
#define OLED_I2C_SCL GPIO_NUM_22
#define OLED_I2C_ADDRESS 0x3C
#define OLED_WIDTH 128
#define OLED_PAGES 8

static const char *TAG = "display";

/* Символы, необходимые для строк HELLO / ESP32 / OLED TEST. */
static const uint8_t font_5x7[128][5] = {
    [' '] = {0x00, 0x00, 0x00, 0x00, 0x00},
    ['2'] = {0x46, 0x49, 0x49, 0x49, 0x31},
    ['3'] = {0x42, 0x49, 0x49, 0x49, 0x36},
    ['D'] = {0x7F, 0x41, 0x41, 0x22, 0x1C},
    ['E'] = {0x7F, 0x49, 0x49, 0x49, 0x41},
    ['H'] = {0x7F, 0x08, 0x08, 0x08, 0x7F},
    ['L'] = {0x7F, 0x40, 0x40, 0x40, 0x40},
    ['O'] = {0x3E, 0x41, 0x41, 0x41, 0x3E},
    ['P'] = {0x7F, 0x09, 0x09, 0x09, 0x06},
    ['S'] = {0x46, 0x49, 0x49, 0x49, 0x31},
    ['T'] = {0x01, 0x01, 0x7F, 0x01, 0x01},
};

/** Отправляет одну команду контроллеру SSD1306. */
static esp_err_t oled_command(uint8_t command)
{
    const uint8_t packet[] = {0x00, command};
    return i2c_master_write_to_device(OLED_I2C_PORT, OLED_I2C_ADDRESS,
                                      packet, sizeof(packet), pdMS_TO_TICKS(100));
}

/** Отправляет одну строку пикселей контроллеру SSD1306. */
static esp_err_t oled_data(const uint8_t *data)
{
    uint8_t packet[OLED_WIDTH + 1] = {0x40};
    for (size_t index = 0; index < OLED_WIDTH; ++index) {
        packet[index + 1] = data[index];
    }
    return i2c_master_write_to_device(OLED_I2C_PORT, OLED_I2C_ADDRESS,
                                      packet, sizeof(packet), pdMS_TO_TICKS(100));
}

/** Настраивает I2C и инициализирует OLED SSD1306. */
static void oled_init(void)
{
    const i2c_config_t config = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = OLED_I2C_SDA,
        .scl_io_num = OLED_I2C_SCL,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 400000,
        .clk_flags = 0,
    };
    ESP_ERROR_CHECK(i2c_param_config(OLED_I2C_PORT, &config));
    ESP_ERROR_CHECK(i2c_driver_install(OLED_I2C_PORT, config.mode, 0, 0, 0));

    const uint8_t commands[] = {
        0xAE, 0xD5, 0x80, 0xA8, 0x3F, 0xD3, 0x00, 0x40,
        0x8D, 0x14, 0x20, 0x00, 0xA1, 0xC8, 0xDA, 0x12,
        0x81, 0x7F, 0xD9, 0xF1, 0xDB, 0x40, 0xA4, 0xA6, 0xAF,
    };
    for (size_t index = 0; index < sizeof(commands); ++index) {
        ESP_ERROR_CHECK(oled_command(commands[index]));
    }
}

/** Переключает OLED на нужную страницу и начало строки. */
static void oled_set_page(uint8_t page)
{
    ESP_ERROR_CHECK(oled_command(0xB0 | page));
    ESP_ERROR_CHECK(oled_command(0x00));
    ESP_ERROR_CHECK(oled_command(0x10));
}

/** Очищает все 8 страниц дисплея. */
static void oled_clear(void)
{
    const uint8_t empty[OLED_WIDTH] = {0};
    for (uint8_t page = 0; page < OLED_PAGES; ++page) {
        oled_set_page(page);
        ESP_ERROR_CHECK(oled_data(empty));
    }
}

/** Выводит маленькую строку шрифтом 5x7 в заданную цветовую зону. */
static void oled_text(const char *text, uint8_t page)
{
    uint8_t line[OLED_WIDTH] = {0};
    size_t position = 0;

    for (size_t index = 0; text[index] != '\0' && position + 6 < OLED_WIDTH;
         ++index) {
        const uint8_t *glyph = font_5x7[(uint8_t)text[index]];
        for (size_t column = 0; column < 5; ++column) {
            line[position++] = glyph[column];
        }
        line[position++] = 0x00;
    }

    oled_set_page(page);
    ESP_ERROR_CHECK(oled_data(line));
}

/** Выводит строку шрифтом 5x7 с увеличением 2x по ширине и высоте. */
static void oled_text_big(const char *text, uint8_t first_page)
{
    uint8_t upper[OLED_WIDTH] = {0};
    uint8_t lower[OLED_WIDTH] = {0};
    size_t position = 0;

    for (size_t index = 0; text[index] != '\0' && position + 12 < OLED_WIDTH;
         ++index) {
        const uint8_t *glyph = font_5x7[(uint8_t)text[index]];
        for (size_t column = 0; column < 5; ++column) {
            for (uint8_t horizontal = 0; horizontal < 2; ++horizontal) {
                uint8_t pixels = 0;
                for (uint8_t row = 0; row < 7; ++row) {
                    if (glyph[column] & (1U << row)) {
                        const uint8_t first_pixel = (uint8_t)(row * 2);
                        if (first_pixel < 8) {
                            pixels |= (uint8_t)(3U << first_pixel);
                        } else {
                            lower[position] |= (uint8_t)(3U << (first_pixel - 8));
                        }
                    }
                }
                upper[position++] = pixels;
            }
        }
        position += 2;
    }

    oled_set_page(first_page);
    ESP_ERROR_CHECK(oled_data(upper));
    oled_set_page((uint8_t)(first_page + 1));
    ESP_ERROR_CHECK(oled_data(lower));
}

/** Заполняет верхнюю половину мелким, а нижнюю крупным шрифтом. */
static void display_hello(void)
{
    oled_clear();

    /* Верхняя половина: четыре плотные строки мелкого шрифта. */
    oled_text("HELLO ESP32 OLED", 0);
    oled_text("HELLO ESP32 OLED", 1);
    oled_text("HELLO ESP32 OLED", 2);
    oled_text("HELLO ESP32 OLED", 3);

    /* Нижняя половина: две большие строки, каждая занимает две страницы. */
    oled_text_big("HELLO", 4);
    oled_text_big("ESP32", 6);
}

/** Точка входа приложения. */
void app_main(void)
{
    ESP_LOGI(TAG, "OLED SSD1306: I2C=0x%02X SDA=%d SCL=%d",
             OLED_I2C_ADDRESS, OLED_I2C_SDA, OLED_I2C_SCL);
    oled_init();
    display_hello();
    ESP_LOGI(TAG, "На OLED выведено: HELLO / ESP32 / OLED TEST");
}
