#include <string.h>
#include <esp_system.h>
#include <esp_log.h>
#include <esp_netif.h>
#include <mdns.h>


#define APP_MDNS_HOSTNAME           CONFIG_APP_MDNS_HOSTNAME
#define APP_MDNS_DESCRIPTION        CONFIG_APP_MDNS_DESCRIPTION

static const char * TAG = "MDNS";


void mdns_initialize(void)
// Initialize mDNS service
{
    //initialize mDNS, do not abort on error
    esp_err_t err = mdns_init();
    if (err) {
        ESP_LOGW(TAG, "mDNS init failed: %d", err);
        return;
    }

    // set hostname and description
    ESP_LOGI(TAG, "setting hostname: %s", APP_MDNS_HOSTNAME);
    mdns_hostname_set(APP_MDNS_HOSTNAME);
    mdns_instance_name_set(APP_MDNS_DESCRIPTION);

    // advertise services
    mdns_service_add(NULL, "_http", "_tcp", 80, NULL, 0);

    ESP_LOGI(TAG, "mDNS initialized");
}


