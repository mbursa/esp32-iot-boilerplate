#include <string.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

//#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#include "events.h"


#define APP_WIFI_SSID                   CONFIG_APP_WIFI_SSID
#define APP_WIFI_PASS                   CONFIG_APP_WIFI_PASSWORD
#define APP_WIFI_RECONNECT_INTERVAL     CONFIG_APP_WIFI_RECONNECT_INTERVAL

static const char *LOG_TAG = "WIFI";


static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT) {
        switch (event_id) {
            case WIFI_EVENT_STA_START:
                esp_wifi_connect();
                break;
            case WIFI_EVENT_STA_DISCONNECTED:
                ESP_LOGI(LOG_TAG, "disconnected");
                esp_event_post(APP_EVENTS, APP_EVENT_WIFI_DISCONNECTED, NULL, 0, portMAX_DELAY);
                esp_wifi_disconnect();
                vTaskDelay(APP_WIFI_RECONNECT_INTERVAL*1000 / portTICK_PERIOD_MS);     // wait until next connection attempt
                ESP_LOGI(LOG_TAG, "trying to reconnect");
                esp_wifi_connect();
                break;
            case WIFI_EVENT_STA_CONNECTED:
                ESP_LOGI(LOG_TAG, "connected");
                break;
            default:
                ESP_LOGI(LOG_TAG, "wifi event id:%d", event_id);
        }
    } 
    else
    if (event_base == IP_EVENT) {
        switch (event_id) {
            case IP_EVENT_STA_GOT_IP:
                ESP_LOGI(LOG_TAG, "WIFI connected, got IP");
                ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
                ESP_LOGI(LOG_TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
                // s_retry_num = 0;
                esp_event_post(APP_EVENTS, APP_EVENT_WIFI_CONNECTED, NULL, 0, portMAX_DELAY);
                break;
            case IP_EVENT_STA_LOST_IP:
                ESP_LOGI(LOG_TAG, "lost IP");
                break;
            default:
                ESP_LOGI(LOG_TAG, "IP event id:%d", event_id);
        }
    }
}

void wifi_initialize(void)
// Initialize WiFi connection
// https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/wifi.html#esp32-wi-fi-station-general-scenario
{
    esp_netif_t* esp_netif = NULL;

    // === 1. Initialization phase
    ESP_LOGI(LOG_TAG, "wifi initialization");

    // create default WiFi interface instance in station mode
    esp_netif = esp_netif_create_default_wifi_sta();
    if (!esp_netif) {
        ESP_LOGE(LOG_TAG, "esp_netif_create_default_wifi_sta() failed");
        abort();
    }

    // === 2. Configuration phase
    ESP_LOGI(LOG_TAG, "wifi configuration");

    // configuration for initialization
    wifi_init_config_t wifi_init_cfg = WIFI_INIT_CONFIG_DEFAULT();
    // connection configuration
    wifi_config_t wifi_connect_cfg = {
        .sta = {
            .ssid     = APP_WIFI_SSID,
            .password = APP_WIFI_PASS,
	     .threshold.authmode = WIFI_AUTH_WPA2_PSK,  // force WPA2_PSK authentication mode
            .pmf_cfg = {
                .capable  = true,
                .required = false
            },
        },
    };

    ESP_ERROR_CHECK(esp_wifi_init(&wifi_init_cfg));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_connect_cfg));
    ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));                             // disable power-saving (for OTA updates)

    // === 3. Register events
    ESP_LOGI(LOG_TAG, "wifi event registrations");

    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        WIFI_EVENT, ESP_EVENT_ANY_ID,
        &wifi_event_handler,
        NULL, NULL
    ));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        IP_EVENT, ESP_EVENT_ANY_ID,
        &wifi_event_handler,
        NULL, NULL
    ));



    // === 4. Start wifi connection
    ESP_LOGI(LOG_TAG, "wifi station start");

    ESP_ERROR_CHECK(esp_wifi_start());
}


