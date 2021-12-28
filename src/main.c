#include <string.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "sdkconfig.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_event.h"

// include application's components
#include "led.h"
#include "mdns.h"
#include "wifi.h"
#include "http.h"

// define aplication's own event name space
ESP_EVENT_DEFINE_BASE(APP_EVENTS);


void app_main()
{
    // initial delay (useful for switchng to the monitoring terminal)
    vTaskDelay(10000 / portTICK_PERIOD_MS);


    // initialize NVS
    ESP_LOGI("MAIN", "NVS init");
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // create a system (default) event loop 
    ESP_LOGI("MAIN", "event loop init");
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // create and initialize LwIP core task
    ESP_LOGI("MAIN", "netif init");
    ESP_ERROR_CHECK(esp_netif_init());

    // application initializations
    ESP_LOGI("MAIN", "application init");
    led_initialize();
    mdns_initialize();
    http_initialize();
    wifi_initialize();  // should be the last
}

