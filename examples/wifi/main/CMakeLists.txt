idf_component_register(SRCS "web_server.c"
                       INCLUDE_DIRS "."
                       REQUIRES esp_driver_gpio esp_wifi esp_netif esp_event esp_http_server nvs_flash)
