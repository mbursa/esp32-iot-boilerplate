#include <string.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"



static const char *LOG_TAG = "LED";

#define BLINK_GPIO 23


static void led_blink_task(void *pvParameter)
{
    static int led_state = 0;
    // Configure the IOMUX register for pad BLINK_GPIO (some pads are
    // muxed to GPIO on reset already, but some default to other
    // functions and need to be switched to GPIO. Consult the
    // Technical Reference for a list of pads and their default
    // functions.)
    gpio_pad_select_gpio(BLINK_GPIO);

    // set the GPIO as an output pin
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);

    while(1) {
        // alter led state
        led_state = led_state ? 0 : 1;
        // set output pin
        gpio_set_level(BLINK_GPIO, led_state);
        // wait
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}





void led_initialize()
{
    // create task for periodic LED blinking
    ESP_LOGI(LOG_TAG, "create task");
    xTaskCreate(&led_blink_task, "led_blink_task", configMINIMAL_STACK_SIZE, NULL, 5, NULL);
}

