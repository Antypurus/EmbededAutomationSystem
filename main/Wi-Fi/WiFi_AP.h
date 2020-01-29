#ifndef WIFI_AP
#define WIFI_AP

#include "esp_wifi.h"
#include "esp_event.h"

#include "freertos/event_groups.h"

#include <string>

#define AP_CHANNEL 0
#define AP_MAX_CONNECTION 1
#define AP_BEACON_INTERVAL 100

class WiFi_AP{
private:
    std::string m_ssid;
    std::string m_password;
    esp_err_t (*event_handler)(void*,system_event_t*);
    EventGroupHandle_t s_wifi_event_group;
    wifi_config_t config = {};
public:
    WiFi_AP(const std::string& ssid, const std::string& password);
    void set_event_handler(esp_err_t event_handler(void*,system_event_t*));
private:
    void configure_ap();
    void init_wifi_access_point();
    static esp_err_t network_event_handler(void* ctx, system_event_t* event);
};


#endif //APP_TEMPLATE_WIFI_AP_H
