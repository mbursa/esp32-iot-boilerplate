#ifndef _SENSOR_H
#define _SENSOR_H

#include "freertos/queue.h"


// structure to hold sensor readings
struct sensor_readings {
    float temp;         // temperature
    float humi;         // humidity
};
typedef struct sensor_readings sensor_readings;


void sensor_initialize();
sensor_readings sensor_last_value();


#endif