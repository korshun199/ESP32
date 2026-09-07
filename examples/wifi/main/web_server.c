/*
 * Второй тест ESP32: собственная Wi-Fi точка доступа и простой HTTP-сервер.
 *
 * После подключения к сети ESP32-Test открыть в браузере:
 * http://192.168.4.1/
 */

#include <stdio.h>

#include "driver/gpio.h"
#include "esp_event.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "nvs_flash.h"

#define LED_GPIO GPIO_NUM_2
#define WIFI_NAME "ESP32-Test"
#define WIFI_PASSWORD "esp32test"

static const char *TAG = "web_server";
static int led_level = 0;

/** Обработчик главной страницы веб-сервера. */
static esp_err_t home_handler(httpd_req_t *request)
{
    const char *page =
        "<!doctype html><html lang='ru'><meta charset='utf-8'>"
        "<meta name='viewport' content='width=device-width,initial-scale=1'>"
        "<title>ESP32 Test</title><body>"
        "<h1>ESP32 работает</h1>"
        "<p>Wi-Fi и web server запущены.</p>"
        "<p>Состояние LED: %s</p>"
        "<form action='/led' method='get'><button>Переключить LED</button></form>"
        "</body></html>";
    char response[512];

    snprintf(response, sizeof(response), page, led_level ? "ON" : "OFF");
    httpd_resp_set_type(request, "text/html; charset=utf-8");
    return httpd_resp_send(request, response, HTTPD_RESP_USE_STRLEN);
}

/** Обработчик переключения светодиода и возврата на главную страницу. */
static esp_err_t led_handler(httpd_req_t *request)
{
    led_level = !led_level;
    ESP_ERROR_CHECK(gpio_set_level(LED_GPIO, led_level));
    httpd_resp_set_status(request, "302 Found");
    httpd_resp_set_hdr(request, "Location", "/");
    return httpd_resp_send(request, NULL, 0);
}

/** Запускает HTTP-сервер с главной страницей и кнопкой LED. */
static void start_web_server(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;

    ESP_ERROR_CHECK(httpd_start(&server, &config));

    const httpd_uri_t home_uri = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = home_handler,
    };
    const httpd_uri_t led_uri = {
        .uri = "/led",
        .method = HTTP_GET,
        .handler = led_handler,
    };

    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &home_uri));
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &led_uri));
    ESP_LOGI(TAG, "HTTP server: http://192.168.4.1/");
}

/** Настраивает ESP32 как точку доступа Wi-Fi. */
static void start_wifi_ap(void)
{
    wifi_init_config_t wifi_config = WIFI_INIT_CONFIG_DEFAULT();
    wifi_config_t ap_config = {
        .ap = {
            .ssid = WIFI_NAME,
            .ssid_len = sizeof(WIFI_NAME) - 1,
            .password = WIFI_PASSWORD,
            .max_connection = 4,
            .authmode = WIFI_AUTH_WPA2_PSK,
        },
    };

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_t *ap_netif = esp_netif_create_default_wifi_ap();
    ESP_ERROR_CHECK(ap_netif != NULL ? ESP_OK : ESP_FAIL);
    ESP_ERROR_CHECK(esp_wifi_init(&wifi_config));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_LOGI(TAG, "Wi-Fi AP: %s, password: %s", WIFI_NAME, WIFI_PASSWORD);
}

/** Инициализирует GPIO, Wi-Fi и HTTP-сервер. */
void app_main(void)
{
    esp_err_t nvs_result = nvs_flash_init();
    if (nvs_result == ESP_ERR_NVS_NO_FREE_PAGES ||
        nvs_result == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        nvs_result = nvs_flash_init();
    }
    ESP_ERROR_CHECK(nvs_result);

    ESP_ERROR_CHECK(gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT));
    ESP_ERROR_CHECK(gpio_set_level(LED_GPIO, 0));
    start_wifi_ap();
    start_web_server();
}
