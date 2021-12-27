#ifndef EVENTS_H_
#define EVENTS_H_

#include "esp_event.h"

// declaration of the application events family
ESP_EVENT_DECLARE_BASE(APP_EVENTS);        

// declaration of specific events under the application event family
enum {                                       
    APP_EVENT_WIFI_CONNECTED,               // wifi has connected
    APP_EVENT_WIFI_DISCONNECTED             // wifi has disconnected
};


#endif // #ifndef EVENT_SOURCE_H_
