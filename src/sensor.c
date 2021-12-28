#include <string.h>
#include <stdio.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

#include "sensor.h"


static const char *LOG_TAG = "SENSOR";
static sensor_readings last_sensor_readings = {
    .temp = NAN,
    .humi = NAN
};

#define APP_SENSOR_READING_INTERVAL     CONFIG_APP_SENSOR_READING_INTERVAL


sensor_readings sensor_last_value()
// Give the last value of the sensor readings.
{
    return last_sensor_readings;
}



static void sensor_read_task(void *pvParameter)
{
    // This component provides faked sensor reading only.
    // If a real sensor (eg. DHT22) sensor shall be used, you need to link a 
    // library that handles the sensor readings and initialize the control pin:
    //     int gpio_pin = 10;
    //     gpio_pad_select_gpio(gpio_pin);
    //     gpio_set_direction(gpio_pin, GPIO_MODE_INPUT);

    while(1) {
        // make fake sensor reading
        ESP_LOGI(LOG_TAG, "reading sensor data");
        float temp = esp_random() / 20;
        float humi = esp_random() / 20;
        bool reading_ok = !isnan(temp) && !isnan(humi);

        // update the variable that sores the last sensor readings
        // a critical section is used that makes sure all interupts are disabled 
        // and that the current task is guaranteed to remain in the running state 
        // until the values are updated
        ESP_LOGI(LOG_TAG, "updating the values");
        portMUX_TYPE myMutex = portMUX_INITIALIZER_UNLOCKED;
        taskENTER_CRITICAL(&myMutex);
        if (reading_ok) {
            last_sensor_readings.temp = (esp_random()/(float)UINT32_MAX)*100.0;
            last_sensor_readings.humi = (esp_random()/(float)UINT32_MAX)*100.0;
        } else {
            last_sensor_readings.temp = NAN;
            last_sensor_readings.humi = NAN;
        }
        taskEXIT_CRITICAL(&myMutex);

        // wait until next reading
        vTaskDelay(APP_SENSOR_READING_INTERVAL*1000 / portTICK_PERIOD_MS);
    }
}



void sensor_initialize()
{
    // create task for periodic sensor readings
    ESP_LOGI(LOG_TAG, "create task");
    xTaskCreate(&sensor_read_task, "sensor_read_task", 2048, NULL, 5, NULL);
}

