#include <string.h>
#include <fcntl.h>
#include "esp_http_server.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_vfs.h"
#include "cJSON.h"

#include "events.h"
#include "sensor.h"

// handle to the HTTP server object
static httpd_handle_t http_server = NULL;


static const char *LOG_TAG = "HTTP";

static esp_err_t http_request_status_get(httpd_req_t *req)
{
    char* json_string = NULL;
    cJSON* json = NULL;
    
    // create json object
    json = cJSON_CreateObject();

    // get sensor readings
    sensor_readings data = sensor_last_value();

    // populate data to json object
    cJSON_AddNumberToObject(json, "temp", data.temp);
    cJSON_AddNumberToObject(json, "humi", data.humi);

    // get json string representation, send it as http response, free memory
    ESP_LOGI(LOG_TAG, "Sending http response to status request");
    json_string = cJSON_Print(json);
    httpd_resp_set_type(req, "application/json");
    httpd_resp_sendstr(req, json_string);
    free((void *)json_string);

    // free json object
    cJSON_Delete(json);

    return ESP_OK;
}


static esp_err_t http_server_start()
//! Start the httpd server.
//! Starts the server and registers request handlers.
{
    assert(http_server == NULL);

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.lru_purge_enable = true;         // force close old connections

    const httpd_uri_t uri_status_get = {
        .uri       = "/status",
        .method    = HTTP_GET,
        .handler   = http_request_status_get,
        .user_ctx  = NULL
    };

    ESP_LOGI(LOG_TAG, "starting server");
    if (httpd_start(&http_server, &config) == ESP_OK) {
        httpd_register_uri_handler(http_server, &uri_status_get);
        ESP_LOGI(LOG_TAG, "server started");
    } else {
        ESP_LOGE(LOG_TAG, "error starting server!");
        http_server = NULL;
        return ESP_FAIL;
    }

    return ESP_OK;
}



static esp_err_t http_server_stop()
//! Stop the httpd server.
{
    assert(http_server);
    esp_err_t res;

    ESP_LOGI(LOG_TAG, "stopping server");
    res = httpd_stop(http_server);
    http_server = NULL;
    ESP_LOGI(LOG_TAG, "server stopped");

    return res;
}



static void http_app_event_handler(void* handler_args, esp_event_base_t event_base, int32_t event_id, void* event_data)
// Handler which executes when the timer started event gets executed by the loop.
{
    if (event_base != APP_EVENTS) return;

    switch (event_id) {
        case APP_EVENT_WIFI_CONNECTED:
            http_server_start();
            break;
        case APP_EVENT_WIFI_DISCONNECTED:
            http_server_stop();
            break;
        default:
            ESP_LOGI(LOG_TAG, "http event id:%d", event_id);
    }
}



void http_initialize()
{
    // register an event handler for the application event group to the default event loop
    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        APP_EVENTS, ESP_EVENT_ANY_ID,
        &http_app_event_handler,
        NULL, NULL
    ));
}

